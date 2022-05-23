var LibraryJsHTTPRequest  = {
    $JsHTTPRequest: {
		myRequestList: [],
		myRequestListSize: 0,
		
	    constructBuffer: function(buffer, result, out_size)
		{
			var mallocbuffer = _malloc(result.length+2);

			HEAP32[out_size>>2] = result.length;
			HEAP32[buffer>>2] = mallocbuffer;

			var bufferpos = mallocbuffer;

			for(var i = 0; i < result.length; i++)
			{
				HEAPU8[bufferpos++] = result.charCodeAt(i);
			}
			HEAPU8[bufferpos++] = 0;
			HEAPU8[bufferpos] = 0;
	   	},
	},

	
	JSSendHTTPRequest: function(a_type, a_url, a_caller, isAsync, a_data, a_data_size)
	{		
		var mytype = UTF8ToString(a_type);
		var myurl = UTF8ToString(a_url);
		var myData;

		if(a_data_size == 0){
			myData = '';
		}else{
			myData = new Uint8Array(a_data_size);
			var data_view = new Uint8Array(Module.HEAP8.buffer, a_data, a_data_size);
			myData.set(data_view);
		}
		
		fetch(myurl)
		.then( (response) => Module.ccall('HTTPAsyncRequestJSParseHeader', // name of C function
											null, // return type
											['number','string'], // argument types
											[a_caller,response])) // arguments)
		.then(  (data)=> Module.ccall('HTTPAsyncRequestJSParseContent', 
					null,
					['number','array','number'],
					[a_caller, data, data.byteLength]))// output will be the required data
		.catch( (error) => console.log(error))
		
		/*jQuery.ajax({
			type: mytype,
			url: myurl,
			data: myData,
			dataType: "binary",
			async: isAsync,
			processData: false,
			responseType:'arraybuffer',
			success: function (response, textvalue, jqXHR) 
			{
				var type = jqXHR.getResponseHeader("Content-Type");
				var encoding = jqXHR.getResponseHeader("Content-Encoding")
				var header=""

				if(type != null){
					header += "Content-Type: " + type + "\n";
				}
				if(encoding != null){
					header += "Content-Encoding: " + encoding + "\n";
				}

				Module.ccall('HTTPAsyncRequestJSParseHeader', // name of C function
				  null, // return type
				  ['number','string'], // argument types
				  [a_caller,header]); // arguments
				
				var buffer = _malloc(response.byteLength);
				var charBuffer = new Uint8Array(Module.HEAP8.buffer, buffer, response.byteLength);
				charBuffer.set(new Uint8Array(response));

				Module.ccall('HTTPAsyncRequestJSParseContent', 
					null,
					['number','array','number'],
					[a_caller, charBuffer, response.byteLength]);
			},
			error: function (request, error, errorThrown) 
			{
				console.log(error + " : " + errorThrown);
				Module.ccall('HTTPAsyncRequestJSParseError', null, ['number','string'], [a_caller, error]);
			},
		})*/
	},

};

autoAddDeps(LibraryJsHTTPRequest, '$JsHTTPRequest');
mergeInto(LibraryManager.library, LibraryJsHTTPRequest);



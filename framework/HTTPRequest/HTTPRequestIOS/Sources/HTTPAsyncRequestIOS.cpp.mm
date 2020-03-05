#include "HTTPAsyncRequestIOS.h"
#include "HTTPConnectIOS.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "AsciiParserUtils.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

IMPLEMENT_CLASS_INFO(HTTPAsyncRequestIOS)

//! constructor
HTTPAsyncRequestIOS::HTTPAsyncRequestIOS(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPAsyncRequest(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}     

//! destructor
HTTPAsyncRequestIOS::~HTTPAsyncRequestIOS()
{
	
}    



void HTTPAsyncRequestIOS::InitModifiable()
{
	HTTPAsyncRequest::InitModifiable();
    
    NSString* L_urlSTR = [[NSString alloc] initWithUTF8String:((HTTPConnect*)((CoreModifiable*)myConnection))->getHostNameWithProtocol().c_str()];

    NSString* L_EndOfurl = [[NSString alloc] initWithUTF8String:myRequestURL.c_str()];
    L_urlSTR = [L_urlSTR stringByAppendingString:L_EndOfurl];
    L_urlSTR = [L_urlSTR stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    L_urlSTR = [L_urlSTR stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    
    
    
    bool L_isSync = false;
    ((CoreModifiable*)myConnection)->getValue(LABEL_TO_ID(IsSynchronous),L_isSync);
    
    NSURLSession* L_Session = [NSURLSession sharedSession];
    if(L_Session)//IOS 7.0 and later
    {
        if(myPostBuffer) //POST REQUEST
        {
            NSString* L_post = [NSString stringWithUTF8String:myPostBuffer];
            NSData* L_PostData = [L_post dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
            
            NSMutableURLRequest* L_Request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:L_urlSTR]];
            [L_Request setHTTPMethod:[NSString stringWithUTF8String:((const kstl::string&)myRequestType).c_str()]];
            [L_Request setValue:[NSString stringWithFormat:@"%i",[L_PostData length]] forHTTPHeaderField:@"Content-Length"];
            [L_Request setHTTPBody:L_PostData];
            
            if(L_isSync)
            {
                NSURLSessionConfiguration* L_config = [NSURLSessionConfiguration defaultSessionConfiguration];
                L_Session = [NSURLSession sessionWithConfiguration:L_config];
                
                
                dispatch_semaphore_t L_semaphore = dispatch_semaphore_create(0);
                
                NSURLSessionDataTask* L_dataTask = [L_Session uploadTaskWithRequest:L_Request fromData:L_PostData completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
                    if(data)
                    {
                        myReceivedRawBufferSize = (unsigned int)[data length];
                        myReceivedRawBuffer = new unsigned char[myReceivedRawBufferSize];
                        memcpy(myReceivedRawBuffer, [data bytes], myReceivedRawBufferSize);
                        NSDictionary* L_header = [reinterpret_cast<NSHTTPURLResponse*>(response) allHeaderFields];
                        NSString* L_headerResult = [L_header objectForKey:@"Content-Encoding"];
                        if([L_headerResult isEqualToString:@"utf-8"])
                            myContentEncoding = UTF8;
                        else if([L_headerResult isEqualToString:@"utf-16"])
                            myContentEncoding = UTF16;
                                                    
                        L_headerResult = [L_header objectForKey:@"Content-Type"];
                        NSRange L_range = [L_headerResult rangeOfString:@"utf-"];
                        if(L_range.location != NSNotFound)
                        {
                            L_headerResult = [L_headerResult substringFromIndex:L_range.location];
                                                    
                            if([L_headerResult isEqualToString:@"utf-8"])
                                myFoundCharset = UTF8;
                            else if([L_headerResult isEqualToString:@"utf-16"])
                                myFoundCharset = UTF16;
                        }
                    }
#ifdef _DEBUG
                    else if (error != nil)
                         NSLog(@"POST synch request Error = %@", error);
#endif
                    dispatch_semaphore_signal(L_semaphore); }];
                
                [L_dataTask resume];
                dispatch_semaphore_wait(L_semaphore, DISPATCH_TIME_FOREVER);
            }
            else
            {
                [[L_Session uploadTaskWithRequest:L_Request fromData:L_PostData completionHandler:^(NSData *data, NSURLResponse *response, NSError *error){
                  if(data)
                  {
                    myReceivedRawBufferSize = (unsigned int)[data length];
                    myReceivedRawBuffer = new unsigned char[myReceivedRawBufferSize];
                    memcpy(myReceivedRawBuffer, [data bytes], myReceivedRawBufferSize);
                  
                    NSDictionary* L_header = [reinterpret_cast<NSHTTPURLResponse*>(response) allHeaderFields];
                    NSString* L_headerResult = [L_header objectForKey:@"Content-Encoding"];
                    if([L_headerResult isEqualToString:@"utf-8"])
                        myContentEncoding = UTF8;
                    else if([L_headerResult isEqualToString:@"utf-16"])
                        myContentEncoding = UTF16;
                  
                    L_headerResult = [L_header objectForKey:@"Content-Type"];
                    NSRange L_range = [L_headerResult rangeOfString:@"utf-"];
                    if(L_range.location != NSNotFound)
                    {
                        L_headerResult = [L_headerResult substringFromIndex:L_range.location];
                  
                        if([L_headerResult isEqualToString:@"utf-8"])
                            myFoundCharset = UTF8;
                        else if([L_headerResult isEqualToString:@"utf-16"])
                            myFoundCharset = UTF16;
                    }
                  }
#ifdef _DEBUG
                  else if (error != nil)
                    NSLog(@"POST asynch request Error = %@", error);
#endif
                  
                  setDone(); }] resume];
            }
        }
        else // GET Request
        {
            if(L_isSync)
            {
                
                dispatch_semaphore_t L_semaphore = dispatch_semaphore_create(0);
                [[L_Session dataTaskWithURL:[NSURL URLWithString:L_urlSTR] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
                    if(data)
                    {
                        myReceivedRawBufferSize = (unsigned int)[data length];
                        myReceivedRawBuffer = new unsigned char[myReceivedRawBufferSize];
                        memcpy(myReceivedRawBuffer, [data bytes], myReceivedRawBufferSize);
                        NSDictionary* L_header = [reinterpret_cast<NSHTTPURLResponse*>(response) allHeaderFields];
                        NSString* L_headerResult = [L_header objectForKey:@"Content-Encoding"];
                        if([L_headerResult isEqualToString:@"utf-8"])
                            myContentEncoding = UTF8;
                        else if([L_headerResult isEqualToString:@"utf-16"])
                            myContentEncoding = UTF16;
                                                    
                        L_headerResult = [L_header objectForKey:@"Content-Type"];
                        NSRange L_range = [L_headerResult rangeOfString:@"utf-"];
                        if(L_range.location != NSNotFound)
                        {
                            L_headerResult = [L_headerResult substringFromIndex:L_range.location];
                                                    
                            if([L_headerResult isEqualToString:@"utf-8"])
                                myFoundCharset = UTF8;
                            else if([L_headerResult isEqualToString:@"utf-16"])
                                myFoundCharset = UTF16;
                        }
                    }
#ifdef _DEBUG
                    else if (error != nil)
                        NSLog(@"GET synch request Error = %@", error);
#endif              
                  
                    dispatch_semaphore_signal(L_semaphore); }] resume];
                
                dispatch_semaphore_wait(L_semaphore, DISPATCH_TIME_FOREVER);
                
            }
            else
            {
                [[L_Session dataTaskWithURL:[NSURL URLWithString:L_urlSTR] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
                  if(data)
                  {
                    myReceivedRawBufferSize = (unsigned int)[data length];
                    myReceivedRawBuffer = new unsigned char[myReceivedRawBufferSize];
                    memcpy(myReceivedRawBuffer, [data bytes], myReceivedRawBufferSize);
                    NSDictionary* L_header = [reinterpret_cast<NSHTTPURLResponse*>(response) allHeaderFields];
                    NSString* L_headerResult = [L_header objectForKey:@"Content-Encoding"];
                    if([L_headerResult isEqualToString:@"utf-8"])
                        myContentEncoding = UTF8;
                    else if([L_headerResult isEqualToString:@"utf-16"])
                        myContentEncoding = UTF16;
                  
                    L_headerResult = [L_header objectForKey:@"Content-Type"];
                    NSRange L_range = [L_headerResult rangeOfString:@"utf-"];
                    if(L_range.location != NSNotFound)
                    {
                        L_headerResult = [L_headerResult substringFromIndex:L_range.location];
                  
                        if([L_headerResult isEqualToString:@"utf-8"])
                            myFoundCharset = UTF8;
                        else if([L_headerResult isEqualToString:@"utf-16"])
                            myFoundCharset = UTF16;
                    }
                  }
#ifdef _DEBUG
                  else if (error != nil)
                        NSLog(@"GET Asynch request Error = %@", error);
#endif
                  
                  setDone();
                  
                  }] resume];
            }
        }
    }
    else //Before IOS 7.0
    {
        NSMutableURLRequest* L_Request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:L_urlSTR]];
        [L_Request setHTTPMethod:[NSString stringWithUTF8String:((const kstl::string&)myRequestType).c_str()]];
        if(myPostBuffer) // POST REQUEST
        {
            NSString* L_post = [NSString stringWithUTF8String:myPostBuffer];
            NSData* L_PostData = [L_post dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
            [L_Request setValue:[NSString stringWithFormat:@"%d",[L_PostData length]] forHTTPHeaderField:@"Content-Length"];
            [L_Request setHTTPBody:L_PostData];
        }

        if(L_isSync)
        {
            NSURLResponse* L_Response = nil;
            NSError* L_Error = nil;
                
            NSData* L_Data = [NSURLConnection sendSynchronousRequest:L_Request returningResponse:&L_Response error:&L_Error];
            if(L_Data)
            {
                myReceivedRawBufferSize = (unsigned int)[L_Data length];
                myReceivedRawBuffer = new unsigned char[myReceivedRawBufferSize];
                memcpy(myReceivedRawBuffer, [L_Data bytes], myReceivedRawBufferSize);
                NSDictionary* L_header = [reinterpret_cast<NSHTTPURLResponse*>(L_Response) allHeaderFields];
                NSString* L_headerResult = [L_header objectForKey:@"Content-Encoding"];
                if([L_headerResult isEqualToString:@"utf-8"])
                    myContentEncoding = UTF8;
                else if([L_headerResult isEqualToString:@"utf-16"])
                    myContentEncoding = UTF16;
                    
                L_headerResult = [L_header objectForKey:@"Content-Type"];
                NSRange L_range = [L_headerResult rangeOfString:@"utf-"];
                if(L_range.location != NSNotFound)
                {
                    L_headerResult = [L_headerResult substringFromIndex:L_range.location];
                        
                    if([L_headerResult isEqualToString:@"utf-8"])
                        myFoundCharset = UTF8;
                    else if([L_headerResult isEqualToString:@"utf-16"])
                        myFoundCharset = UTF16;
                }
            }
        }
        else
        {
            NSOperationQueue* L_Queue = [[NSOperationQueue alloc] init];
            [NSURLConnection sendAsynchronousRequest:L_Request queue:L_Queue completionHandler:^(NSURLResponse* response, NSData *data, NSError *error){
                if(data)
                {
                    myReceivedRawBufferSize = (unsigned int)[data length];
                    myReceivedRawBuffer = new unsigned char[myReceivedRawBufferSize];
                    memcpy(myReceivedRawBuffer, [data bytes], myReceivedRawBufferSize);
                    NSDictionary* L_header = [reinterpret_cast<NSHTTPURLResponse*>(response) allHeaderFields];
                    NSString* L_headerResult = [L_header objectForKey:@"Content-Encoding"];
                    if([L_headerResult isEqualToString:@"utf-8"])
                        myContentEncoding = UTF8;
                    else if([L_headerResult isEqualToString:@"utf-16"])
                        myContentEncoding = UTF16;
                 
                    L_headerResult = [L_header objectForKey:@"Content-Type"];
                    NSRange L_range = [L_headerResult rangeOfString:@"utf-"];
                    if(L_range.location != NSNotFound)
                    {
                        L_headerResult = [L_headerResult substringFromIndex:L_range.location];
                 
                        if([L_headerResult isEqualToString:@"utf-8"])
                            myFoundCharset = UTF8;
                        else if([L_headerResult isEqualToString:@"utf-16"])
                            myFoundCharset = UTF16;
                    }
                }
                setDone();}];
        }
    }
}

void HTTPAsyncRequestIOS::protectedProcess()
{
	HTTPAsyncRequest::protectedProcess();
}


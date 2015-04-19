//
//  FileOperation.m
//  ChainMonkey
//
//  Created by TinyUlt on 14-9-20.
//
//

#import "FileOperation.h"
#import "ADDL.h"
#include "cocos2d.h"
USING_NS_CC;
void FileOperation::addAd(){
    
    isIAD = false;
    cocos2d::CCSize size = cocos2d::CCDirector::sharedDirector()->getWinSize();
    NSString* versionString = [[UIDevice currentDevice] systemVersion];
    int verNum = [versionString intValue];
    
    if (verNum >= 4) {
        isIAD = true;
        // create and setup adview
        dl=[[ADDL alloc] init];
        adView = [[ADBannerView alloc] initWithFrame:CGRectZero];
        ((ADBannerView *)adView).requiredContentSizeIdentifiers = [NSSet setWithObjects: ADBannerContentSizeIdentifier320x50 , nil];
        
        
#ifdef vertically
        ((ADBannerView *)adView).currentContentSizeIdentifier = ADBannerContentSizeIdentifier320x50;
#else
        ((ADBannerView *)adView).currentContentSizeIdentifier = ADBannerContentSizeIdentifier480x32;
        //((ADBannerView *)adView).transform = CGAffineTransformMakeRotation(CC_DEGREES_TO_RADIANS(90));
#endif
        
        
        ((ADBannerView *)adView).center = CGPointMake( ((ADBannerView *)FileOperation::adView).frame.size.width/2, -((ADBannerView *)FileOperation::adView).frame.size.height/2);//起始位置
        ((ADBannerView *)adView).delegate =(ADDL *)dl;
        bannerIsVisible =false;
        [(CCEGLView *)view addSubview: (ADBannerView *)adView];
        
    }
    
    
//    auto adView = [[ADBannerView alloc]initWithFrame:
//                   CGRectMake(0, 0, 480, 32)];
//    ((ADBannerView *)adView).delegate =(ADDL *)dl;
//    bannerIsVisible =false;
    
    
//    // Optional to set background color to clear color
//    [adView setBackgroundColor:[UIColor clearColor]];
//    //bannerView.currentContentSizeIdentifier = ADBannerContentSizeIdentifier480x32;
//    
//    [(CCEGLView *)view addSubview: (ADBannerView *)adView];
    
}
void FileOperation::deletAd(){
    if(isIAD){
        ((ADBannerView *)adView).delegate = nil;
        [((ADBannerView *)adView) removeFromSuperview];
        [((ADBannerView *)adView) release];
        adView = nil;
        [((ADDL *)dl) relaease];
        isIAD = false;
    }
    
}
void *  FileOperation::dl;
void *   FileOperation::adView;
bool  FileOperation::bannerIsVisible;
bool  FileOperation::isIAD;
void * FileOperation::view;


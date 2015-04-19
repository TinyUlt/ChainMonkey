//  ADDL.m
//  BirdAndEggLite
//
//  Created by kinet on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#import "ADDL.h"
#import "FileOperation.h"
#include "cocos2d.h"


@implementation ADDL
- (id)init{
    self = [super init];
    if (self) {
        
    }
    
    return self;
}
-(void)relaease
{
    [super dealloc];
}
- (void)dealloc
{
    
    [super dealloc];
}
-(void) bannerView:(ADBannerView *)banner didFailToReceiveAdWithError:(NSError *)error
{
    
    
    if (FileOperation::bannerIsVisible) {
        [UIView beginAnimations:@"animateAdBannerOff" context:NULL];
        // banner is visible and we move it out of the screen, due to connection issue
        ((ADBannerView *)FileOperation::adView).center = OUT_POS;
        [UIView commitAnimations];
        FileOperation::bannerIsVisible = false;
    }
}
-(void) willRotateToInterfaceOrientation:(UIInterfaceOrientation) toInterfaceOrientation duration:(NSTimeInterval)duration
{
    //adView.frame = CGRectZero;
    if (UIInterfaceOrientationIsLandscape(toInterfaceOrientation)) {
        ((ADBannerView *)FileOperation::adView).currentContentSizeIdentifier = ADBannerContentSizeIdentifier480x32;
        ((ADBannerView *)FileOperation::adView).frame = CGRectZero;
    } else {
        ((ADBannerView *)FileOperation::adView).currentContentSizeIdentifier = ADBannerContentSizeIdentifier320x50;
        ((ADBannerView *)FileOperation::adView).frame = CGRectZero;
    }
}
-(void) bannerViewDidLoadAd:(ADBannerView *)banner
{
    NSLog(@"ad: bannerViewDidLoadAd");
    
    if (!FileOperation::bannerIsVisible) {
        [UIView beginAnimations:@"animateAdBannerOn" context:NULL];
        // banner is invisible now and moved out of the screen on 50 px
        cocos2d::CCSize size = cocos2d::CCDirector::sharedDirector()->getWinSize();
#ifdef vertically
        ((ADBannerView *)FileOperation::adView).center = CGPointMake( ((ADBannerView *)FileOperation::adView).frame.size.width/2, size.height-25);
#else
        ((ADBannerView *)FileOperation::adView).center = CGPointMake( ((ADBannerView *)FileOperation::adView).frame.size.width/2, size.height/2-((ADBannerView *)FileOperation::adView).frame.size.height/2);
#endif
        [UIView commitAnimations];
        FileOperation::bannerIsVisible = true;
    }
}

@end
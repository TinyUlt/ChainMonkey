//
//  FileOperation.h
//  ChainMonkey
//
//  Created by TinyUlt on 14-9-20.
//
//

class FileOperation
{
public:
    static  void addAd();//添加iad
    static  void deletAd();//删除iad
    static void * view;//EGLView指针
    static void * adView;//广告
    static void * dl;//广告的delegate
    static bool bannerIsVisible;//设置是否可见
    static bool isIAD;//是否创建了广告
};
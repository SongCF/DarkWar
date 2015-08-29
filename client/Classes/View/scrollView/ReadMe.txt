这里面时自己封装的一个ScrollView
可以轻松实现选章节，选关卡的页面。

这里面使用最多，也最有用，最常用的就是GridScrollView了，他把内容分为了有页面大小，页面间隔，

下面是个例子：

// 构造时默认会移动到屏幕中心
// 所有如果你不想他移动，就在构造完毕之后设置一次坐标即可
heroGallery = new GridScrollView(
	CCSizeMake(640,960),
	sizeof(actorType)/sizeof(int),	// 元素的个数
	1,								// 每页横向显示几个
	1,								// 每页纵向显示几个
	10.0f,
	CCPointZero,
	20.0f);
addChild(heroGallery);
heroGallery->release();
heroGallery->setClickableCreater(this);
// 可以显示了
heroGallery->prepareToDisplay();
heroGallery->setPosition(CCPointZero);
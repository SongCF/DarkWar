������ʱ�Լ���װ��һ��ScrollView
��������ʵ��ѡ�½ڣ�ѡ�ؿ���ҳ�档

������ʹ����࣬Ҳ�����ã���õľ���GridScrollView�ˣ��������ݷ�Ϊ����ҳ���С��ҳ������

�����Ǹ����ӣ�

// ����ʱĬ�ϻ��ƶ�����Ļ����
// ��������㲻�����ƶ������ڹ������֮������һ�����꼴��
heroGallery = new GridScrollView(
	CCSizeMake(640,960),
	sizeof(actorType)/sizeof(int),	// Ԫ�صĸ���
	1,								// ÿҳ������ʾ����
	1,								// ÿҳ������ʾ����
	10.0f,
	CCPointZero,
	20.0f);
addChild(heroGallery);
heroGallery->release();
heroGallery->setClickableCreater(this);
// ������ʾ��
heroGallery->prepareToDisplay();
heroGallery->setPosition(CCPointZero);
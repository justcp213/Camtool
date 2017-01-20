int addBuff(unsigned char b)
{
	static index = 0;

	buff[index] = b;
	
	buff_length = index++;

	return buff_length;

}



void repeat_block(char o)
{
	int i=0, l=5;

	for(i=0; i<l; i++)
	{
		addBuff(0x80);
		addBuff(o); 
		addBuff(0x03); 
	}
}

void repeat_block2(char o)
{
	
	addBuff(0x80);
	addBuff(o); 
	addBuff(0x03); 

}

void clock()
{
	repeat_block(save_state & 0xfe);
	repeat_block(save_state | 0x01);
	repeat_block(save_state & 0xfe);
}

void clock_bit(char b)
{
	if(b)
	{
		repeat_block(0x02);
		save_state = 0x02;
	}
	else
	{
		repeat_block(0x00);
		save_state = 0x00;
	}

	clock();
}
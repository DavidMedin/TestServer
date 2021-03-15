#pragma once
typedef enum{
	Quit,
	DisplayText
}MessageType;
//PACKET HEADER FORMAT
/*
{
	MessageType (4 bytes), //nothing after this is needed if MessageType is Quit
	unsigned int sizeOfMessage,
	bla bla your data here
}
*/
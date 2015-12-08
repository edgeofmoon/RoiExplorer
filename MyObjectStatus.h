#pragma once
class MyObjectStatus
{
public:
	MyObjectStatus(){ mStatusByte = STATUS_DEFAULT; };
	~MyObjectStatus(){};

	enum Status{
		STATUS_DEFAULT = 0,
		STATUS_DISABLE_BIT = 1,
		STATUS_SELECT_BIT = 2,
	};
	void SetDefault(){
		mStatusByte = STATUS_DEFAULT;
	}
	void SetStatusBit(int byte){
		mStatusByte |= byte;
	}
	void UnsetStatusBit(int byte){
		mStatusByte &= ~byte;
	}
	bool IsBitSet(int byte) const {
		return (mStatusByte&byte) == byte;
	}

	bool operator==(const MyObjectStatus::Status& status) const{
		return mStatusByte == status;
	}

protected:
	int mStatusByte;
};


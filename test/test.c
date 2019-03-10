int main(){
	int result = 0;
	int i = 1;

	if(i > 2){
		result = result + 3;
	}else{
		result = result + 4;
	}

	for(i = 0; i < 10; i = i + 1){
		result = result + i;
	}

	printf("result: %d\n",result);
}

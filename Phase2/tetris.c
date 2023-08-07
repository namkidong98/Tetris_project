#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr(); //ncurses 라이브러리 사용(screen을 생성)
	noecho(); //입력을 화면에 출력 X
	keypad(stdscr, TRUE); // 키패드와 특수문자의 입력을 가능하게 함

	srand((unsigned int)time(NULL)); //매번 다른 난수 생성
	createRankList();
	while(!exit){ //menu 함수를 호출하여 4번(exit)이 들어오기 전까지 반복시행
		clear(); //화면에 그려져 있는 모든 내용을 지운다
		switch(menu()){ 
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		//case MENU_REC: 
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	Node_pointer ptr; Node_pointer temp;
	//노드들 다 지우기
	for (ptr = head; ptr != NULL; ) {
		temp = ptr;
		ptr = ptr->link;
		free(temp);
	}

	endwin(); //initscr의 대응되는 것으로
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++) //field의 모든 값을 0으로 설정
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0] = rand() % 7; //현재 블록 무작위
	nextBlock[1] = rand() % 7; //다음 블록 무작위 생성
	nextBlock[2] = rand() % 7; 

	blockRotate=0; //회전수는 0으로 초기화
	blockY=-1; //시작지점은 y좌표는 -1(0보다 위)
	blockX=WIDTH/2-2; //x는 너비의 중간 (-2는 블록이 4x4니깐 2칸전부터 그려야 중간에 그려짐)
	score=0;	//점수는 0으로 초기화
	gameOver=0; //gameover flag도 0으로 초기화 --> 1이되면 화면에 gameover 띄우고 게임 종료
	timed_out=0; //

	DrawOutline(); //테두리 그리기
	DrawField(); //필드 그리기

	DrawShadow(blockY, blockX, nextBlock[0], blockRotate);
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' '); //현재 블록 그리기

	DrawNextBlock(nextBlock); //다음 블록을 생성
	PrintScore(score); //
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH); //height와 width 값

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10); //width보다 옆으로 나아가므로 
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8); //세로4, 가로8의 크기로 박스그리기

	DrawBox(9, WIDTH + 10, 4, 8); //두번째 next block을 넣을 칸 그리기

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8); //세로1, 가로 8의 박스 그리기
}

int GetCommand(){
	int command;
	command = wgetch(stdscr); //키보드에 받은 입력을 리턴
	switch(command){
	case KEY_UP: //ncurses를 통해 위쪽 화살표를 상수로 받아옴
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q': //break가 없어서 다음 case의 결과물을 시행하게 됨?
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}

void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){ //4x4 사이즈의 칸마다 조사하여 1이면 채워진거, 0이면 빈 것으로 간주
	int i, j;

	//첫번째 nextBlock 그리기
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13); //다음 블록을 그리라고 만들어 둔 테두리로 이동
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){ //채워져있다면
				attron(A_REVERSE); //색 반전
				printw(" "); //빈공간
				attroff(A_REVERSE); //색 반전 끄기
			}
			else printw(" "); //빈공간은 공백으로 표시
		}
	}

	//두번째 nextBlock을 그리기
	for (i = 0; i < 4; i++) {
		move(10 + i, WIDTH + 13); //다음 블록을 그리라고 만들어 둔 테두리로 이동
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) { //채워져있다면
				attron(A_REVERSE); //색 반전
				printw(" "); //빈공간
				attroff(A_REVERSE); //색 반전 끄기
			}
			else printw(" "); //빈공간은 공백으로 표시
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){ //i+y>=0 은 무엇을 위한 것인가?
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x); 
	addch(ACS_ULCORNER); //upleft 좌상단모서리
	for(i=0;i<width;i++) //너비만큼
		addch(ACS_HLINE); ////가로라인을 현재 커서 위치에 출력
	addch(ACS_URCORNER); //upright 우상단모서리
	for(j=0;j<height;j++){
		//좌측라인 그리기
		move(y+j+1,x); 
		addch(ACS_VLINE); //세로라인
		
		//우측라인 그리기
		move(y+j+1,x+width+1); 
		addch(ACS_VLINE); //세로라인
	}
	move(y+j+1,x); //좌측구석으로 이동
	addch(ACS_LLCORNER); //lowleft 좌하단모서리
	for(i=0;i<width;i++)
		addch(ACS_HLINE); //하단그리기
	addch(ACS_LRCORNER); //lowright 우하단모서리
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1); //1초뒤에 blockdown함수를 호출(예약)
			timed_out=1;//루프를 다시 돌 수 있도록 변경해놓기
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh(); //변경 사항을 윈도우 화면에 적용
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh(); //여까지해서 반영
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if ((i + blockY >= HEIGHT) || (j + blockX >= WIDTH) || (j + blockX < 0)) return 0; //필드 바깥으로 나갈 경우
				if (f[i + blockY][j + blockX] == 1) return 0; //필드와 충돌하는 경우
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int pre_Rotate = 0;
	int pre_blockY = 0;
	int pre_blockX = 0;
	int tmp_blockY = blockY;
	switch (command) {
	case KEY_UP:
		pre_Rotate = (blockRotate + 3) % 4;
		while (CheckToMove(field, currentBlock, pre_Rotate, tmp_blockY + 1, blockX)) tmp_blockY++;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (block[currentBlock][pre_Rotate][i][j] == 1) {
					move(i + blockY + 1, j + blockX + 1);
					printw("%c", '.');
					move(i + tmp_blockY + 1, j + blockX + 1);
					printw("%c", '.');
				}
			}
		}
		break;
	case KEY_DOWN:
		pre_blockY = blockY - 1;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (block[currentBlock][blockRotate][i][j] == 1) {
					move(i + pre_blockY + 1, j + blockX + 1);
					printw("%c", '.');
				}
			}
		}
		break;
	case KEY_RIGHT:
		pre_blockX = blockX - 1;
		while (CheckToMove(field, currentBlock, blockRotate, tmp_blockY + 1, pre_blockX)) tmp_blockY++;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (block[currentBlock][blockRotate][i][j] == 1) {
					move(i + blockY + 1, j + pre_blockX + 1);
					printw("%c", '.');
					move(i + tmp_blockY + 1, j + pre_blockX + 1);
					printw("%c", '.');
				}
			}
		}
		
		break;
	case KEY_LEFT:
		pre_blockX = blockX + 1;
		while (CheckToMove(field, currentBlock, blockRotate, tmp_blockY + 1, pre_blockX)) tmp_blockY++;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (block[currentBlock][blockRotate][i][j] == 1) {
					move(i + blockY + 1, j + pre_blockX + 1);
					printw("%c", '.');
					move(i + tmp_blockY + 1, j + pre_blockX + 1);
					printw("%c", '.');
				}
			}
		}
	}
	DrawShadow(blockY, blockX, currentBlock, blockRotate);
	DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');
}

void BlockDown(int sig){
	int i, j;
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		for (i = 0; i < 4; i++) { //이전 블록 지우기
			for (j = 0; j < 4; j++) {
				if (block[nextBlock[0]][blockRotate][i][j] == 1) {
					move(i + blockY + 1, j + blockX + 1);
					printw("%c", '.');
				}
			}
		}
		blockY++; //한 칸 내리고
		DrawShadow(blockY, blockX, nextBlock[0], blockRotate);
		DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' '); //해당 위치로 블록 그리기
	}
	else {
		if (blockY == -1) gameOver = 1;
		else {
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			
			//다음 블록을 현재 블록에 넣고 다음 블록에는 무작위 블록 할당
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			
			//현재 블록의 초기화(InitTetris와 동일)
			blockRotate = 0;
			blockY = -1;
			blockX = WIDTH / 2 - 2;

			DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
		}
	}
	timed_out = 0; //계속 떨어질 수 있도록 값을 변경해놓음
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j; int touched = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				f[blockY + i][blockX + j] = 1;
				if (f[blockY + i + 1][blockX + j] == 1) touched++;
				else if ((blockY + i) == (HEIGHT - 1)) touched++;
			}
		}
	}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, k; int count = 0;
	for (i = 0; i < HEIGHT; i++) {
		int deleteflag = 1;
		for (j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0) deleteflag = 0;
		}
		if (deleteflag == 1) {
			count++;
			for (j = 0; j < WIDTH; j++) f[i][j] = 0; //꽉찬 해당 줄을 삭제
			
			//위의 칸들을 한칸씩 아래로 이동
			for (k = i; k >= 0; k--) {
				for (j = 0; j < WIDTH; j++) { 
					f[k][j] = f[k - 1][j];
				}
			}
			
		}
	}
	return count * count * 100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int tmp_blockY = blockY;
	while (CheckToMove(field, blockID, blockRotate, tmp_blockY + 1, blockX)) {
		tmp_blockY++;
	}
	DrawBlock(tmp_blockY, blockX, blockID, blockRotate, '/');
}

void InsertNode(char name[], int score) {
	Node_pointer temp; Node_pointer ptr; Node_pointer trail = NULL;
	temp = (Node_pointer)malloc(sizeof(Node));
	strcpy(temp->name, name);
	temp->score = score;
	if (head == NULL) {
		temp->link = head;
		head = temp;
	}
	else {
		for (ptr = head; ptr != NULL; ptr = ptr->link) trail = ptr;
		trail->link = temp;
		temp->link = NULL;
	}
}

void createRankList() {
	FILE* fp;
	int i, j;
	char str[NAMELEN + 1];
	int sc;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");

	// 2. 정보읽어오기
	fscanf(fp, "%d", &score_number);
	for (i = 0; i < score_number; i++) {
		if (fscanf(fp, "%s %d\n", str, &sc) != EOF) { // EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
			InsertNode(str, sc);
		}
		else break;
	}
	
	// 4. 파일닫기
	fclose(fp);
}

void rank() {
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화 (X의 값은 default 1이고 Y의 값은 default score_number이다 )
	int X = 1, Y = score_number, ch, i, j; Node_pointer ptr = NULL; Node_pointer trail = NULL;
	int tmp; int result = 0;
	clear();

	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by specific name\n");
	printw("3. delete a specific rank\n");
	ch = wgetch(stdscr);
	
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		echo();

		printw("X: ");
		result = scanw("%d", &tmp);
		if (result != -1) X = tmp;
		
		move(4, 0);
		printw("Y: ");
		result = scanw("%d", &tmp);
		if (result != -1) Y = tmp;
		
		noecho();

		move(5, 0);
		printw("       name       |   score   ");
		move(6, 0);
		printw("------------------------------\n");

		if (X <= Y && X >= 1 && Y <= score_number) {
			for (i = 1, ptr = head; i < X; i++) ptr = ptr->link;
			for (j = 0; j <= Y - X; j++) {
				printw(" %-17s", ptr->name);
				printw("| %-10d\n", ptr->score);
				ptr = ptr->link;
			}
		}
		else {
			printw("\nsearch failure: no rank in the list");
		}
	}

	//테트리스 2주차 과제
	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2') {
		char str[NAMELEN + 1];
		int check = 0;
		echo();
		move(3, 0);
		printw("Input the name: ");
		scanw("%s", str);
		noecho();
		move(4, 0);
		printw("       name       |   score   ");
		move(5, 0);
		printw("------------------------------\n");
		
		for (ptr = head; ptr != NULL; ptr = ptr->link) {
			if (strcmp(ptr->name, str) == 0) {
				printw(" %-17s", ptr->name);
				printw("| %-10d\n", ptr->score);
				check = 1;
			}
		}
		if(check == 0) printw("\nsearch failure: no name in the list");
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if (ch == '3') {
		int num; 
		echo();
		move(3, 0);
		printw("Input the rank: ");
		scanw("%d", &num);
		noecho();
		move(5, 0);

		if (num <= score_number && num >= 1) {
			ptr = head;
			if (num >= 2) {
				for (i = 2; i <= num; i++) {
					trail = ptr;
					ptr = ptr->link;
				}
				trail->link = ptr->link;
			}
			else head = ptr->link;

			free(ptr);
			score_number--;
			writeRankFile();
			printw("result: the rank deleted");
		}
		else printw("search failure: no name in the list");
	}
	getch(); //함수가 끝나지 않도록 멈추게 하는 역할
}

void writeRankFile() {
	int sn, i; Node_pointer ptr;
	FILE* fp = fopen("rank.txt", "r");

	fscanf(fp, "%d", &sn);
	if (sn == score_number) fclose(fp);
	else {
		fclose(fp);
		fp = fopen("rank.txt", "w");
		fprintf(fp, "%d\n", score_number);
		ptr = head;
		for (i = 0; i < score_number; i++) {
			fprintf(fp, "%s %d\n", ptr->name, ptr->score);
			ptr = ptr->link;
		}
		fclose(fp);
	}
}


void newRank(int score) {
	char str[NAMELEN + 1];
	int i, j;
	clear();

	//1. 사용자 이름을 입력 받음
	move(0, 0);
	printw("your name: ");
	echo();
	scanw("%s", str);
	noecho();

	Node_pointer temp;
	temp = (Node_pointer)malloc(sizeof(Node));
	strcpy(temp->name, str);
	temp->score = score;

	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	if (score_number > 0) {
		Node_pointer ptr;
		Node_pointer trail;
		for (ptr = head; ptr != NULL; ptr = ptr->link) {
			if (score > ptr->score) break;
			trail = ptr;
		}
		trail->link = temp;
		temp->link = ptr;
	}
	else {
		head = temp;
		temp->link = NULL;
	}
	score_number++;
	writeRankFile();
}

/*
void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
*/
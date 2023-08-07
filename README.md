# Tetris_project
Tetris project for Sogang Univ, 2022_2nd semester, CSE3013

<br/>

## Tetris_Final: 추천 시스템과 랭크 시스템을 갖춘 테트리스 프로그램을 만든다
![tetris](https://github.com/namkidong98/Maze_project/assets/113520117/ad921e75-b179-44c6-a10e-482f09a3275f)
1. phase1: 기본 테트리스 프로그램을 만들고 다음 블록과 점수판이 보이도록 한다
2. phase2: linked list 구조로 랭킹 데이터를 관리하며 검색, 삭제 기능을 갖춘 랭킹 시스템을 구현한다
3. phase3: 점수를 높이는 위치를 tree 구조로 예측하는 추천 시스템을 구현하고 이를 바탕으로 recommend play 옵션을 추가한다

<br/><br/>

## phase1: 기본 테트리스 프로그램을 만든다
![1](https://github.com/namkidong98/Tetris_project/assets/113520117/a154b3fb-9e14-4a55-8b2c-3e83444ae332)
1. 방향키에 따라 주어진 필드에서 블록을 움직일 수 있다
2. 블록이 떨어질 위치에 그림자를 표시하고 기존 블록에 닿는 면적 당, 없앤 줄의 개수 당 점수를 올린다 

<br/><br/>

## phase2: 테트리스 프로그램에 랭킹 시스템을 추가한다
![image](https://github.com/namkidong98/Tetris_project/assets/113520117/c945cff1-5857-48f0-acb9-bf213854d53b)
1. 처음 프로그램 실행 시, rank.txt 파일의 데이터를 읽어 랭킹 데이터를 Linked List로 저장한다
2. 높은 점수 순서대로 랭크를 관리하고 랭킹 순위 검색, 이름 검색, 삭제 기능을 구현한다
3. 프로그램이 종료 시, rank.txt 파일에 랭킹 데이터를 입력한다

<br/><br/>

## phase3: 테트리스 프로그램에 추천 시스템을 추가한다
![image](https://github.com/namkidong98/Tetris_project/assets/113520117/15747f44-12ba-498a-bc61-ba2ecd1dc474)
1. Tree구조를 활용하여 현재 블록을 놓을 수 있는 위치만큼 child node를 만들고 최고 점수에 도달하는 경우의 수를 계산한다
2. 그러나 tree의 depth가 늘어남에 따라 child node가 기하급수적으로 많아져 속도가 느려지는 단점이 있다
3. tree의 비효율성을 개선하기 위해 가중치를 적용하여 불필요한 child node들을 미리 제거하는 방식을 구현한다
4. 위의 알고리즘을 기반으로 자동으로 블록을 추천된 위치에 놓는 recommend play를 구현한다

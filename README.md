<a href="https://github.com/Jeong-Je/torrent-clone/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Jeong-Je/torrent-clone" />
</a>


# 리눅스에서 C언어로 토렌트 구현하기 냥!
<div style="display: flex;">
  <img src="https://img.shields.io/badge/Linux-FFE005?style=flat&logo=Linux&logoColor=black" width="150" height="50" />
  <img src="https://img.shields.io/badge/C-00599C?style=flat&logo=C&logoColor=white" width="150" height="50" />
  <img src="https://img.shields.io/badge/BitTorrent-6C47FF?style=flat&logo=BitTorrent&logoColor=white" width="150" height="50" />
</div>



## Client
### 실행 파일 만들기
```
make
```
### `.torrent` 파일 만들기
```
./torrent make <원본 파일 경로>
```
1. [Bencode](https://en.wikipedia.org/wiki/Bencode) 규칙에 따라 **메타데이터(.torrent)** 생성
2. **메타데이터** 생성이 완료되면 **트래커** 서버에 최초 시드임을 보고
<br />

### `.torrent`로 파일 다운받기
```
./torrent down <.torrent 파일 경로>
```
1. [Bencode](https://en.wikipedia.org/wiki/Bencode)된 **.torrent** 파일을 디코딩하여 **메타데이터** 확보
2. 메타데이터에서 트래커 서버 주소 확인 후 시드 정보 요청
3. 시드의 개수에 따라 **스레드** 생성
4. 스레드 함수에서 시드들과 **P2P** 통신으로 파일 다운로드
***
## Tracker

### 실행 파일 만들기
```
make
```
1. 트래커는 클라이언트의 요청을 위해 항상 **Listen** 상태
2. 클라이언트 요청이 오면 **스레드** 생성
3. 스레드 함수에서 요청을 처리

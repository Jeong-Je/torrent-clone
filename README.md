
![bit_torrent](https://github.com/user-attachments/assets/606e1afe-7a19-4b35-bae5-e2991693cd8b)

# C언어로 토렌트 구현하기
***
## Client

### `.torrent` 파일 생성하기
```
./torrent make <원본 파일 경로>
```
1. [Bencode](https://en.wikipedia.org/wiki/Bencode) 규칙을 따라 .torrent 파일 생성
2. `.torrent` 파일이 생성이 완료되면 트래커 서버로 최초 시드로 보고

<br />
<br />

### `.torrent`로 파일 다운하기
```
./torrent down <.torrent 파일 경로>
```
1. [Bencode](https://en.wikipedia.org/wiki/Bencode)된 `.torrent` 파일을 디코딩하여 메타데이터 확보
2. 메타데이터에 기록된 트래커 서버로 시드 주소 요청

***

## Tracker



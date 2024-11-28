# 리눅스에서 C언어로 토렌트 구현하기
<div style="display: flex;">
  <img src="https://img.shields.io/badge/Linux-FFE005?style=flat&logo=Linux&logoColor=black" width="150" height="50" />
  <img src="https://img.shields.io/badge/C-00599C?style=flat&logo=C&logoColor=white" width="150" height="50" />
  <img src="https://img.shields.io/badge/BitTorrent-6C47FF?style=flat&logo=BitTorrent&logoColor=white" width="150" height="50" />
</div>



## Client
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
1. [Bencode](https://en.wikipedia.org/wiki/Bencode)된 `.torrent` 파일을 디코딩하여 메타데이터 확보
2. 메타데이터에서 트래커 서버 주소 확인 후 시드 정보 요청
3. 시드들과 `P2P` 통신으로 파일 다운로드

## Tracker

ㅋㅋ

# 'a'로 1333395456바이트를 채운 파일 생성
dd if=/dev/zero bs=1M count=1277 | tr '\0' 'a' > part_a.txt

# 'b'로 1333395456바이트를 채운 파일 생성
dd if=/dev/zero bs=1M count=1277 | tr '\0' 'b' > part_b.txt

# 'c'로 1333209088바이트를 채운 파일 생성
dd if=/dev/zero bs=1M count=1276 | tr '\0' 'c' > part_c.txt

# 세 파일을 결합하여 최종 파일을 생성
cat part_a.txt part_b.txt part_c.txt > output.txt

# 생성된 파일을 확인 (파일 크기)
ls -lh output.txt

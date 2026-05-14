# BÁO CÁO THỰC HÀNH LAB 05

## Xây dựng ứng dụng AI cơ bản trong mô hình IoT (Nhận diện khuôn mặt với FaceNet)

**Học phần:** Công nghệ Internet of Things Hiện đại  
**Khoa:** Mạng máy tính & Truyền thông – UIT  
**GVHD:** Phan Trung Phát (phatpt@uit.edu.vn)

**Nhóm:** [NhomY]  
**Thành viên:**

- [Họ tên – MSSV]
- [Họ tên – MSSV]

**Ngày thực hiện:** [dd/mm/yyyy]

> Gợi ý nộp bài theo PDF: nén toàn bộ và đặt tên `NhomY-Lab05_MSSV1_MSSV2`.

---

## Tóm tắt

Báo cáo mô tả quá trình triển khai một ứng dụng **nhận diện khuôn mặt** theo mô hình **IoT + Edge computing**. Thiết bị **Raspberry Pi 3** đảm nhiệm thu ảnh từ camera và gửi ảnh qua Internet đến **edge server (PC/Laptop)**. Edge server thực hiện tiền xử lý (phát hiện/căn chỉnh khuôn mặt), trích xuất đặc trưng bằng mô hình **FaceNet** và huấn luyện/nhận diện bằng **classifier**. Kết quả nhận diện được trả ngược về Raspberry Pi để hiển thị. Hệ thống được đánh giá bằng **độ chính xác nhận diện** (yêu cầu > 60% theo PDF) và **độ trễ end-to-end** (tính từ lúc Raspberry Pi gửi ảnh đến lúc nhận kết quả trả về).

> Lưu ý: các trường có dạng `[điền]`/`[ ]` là thông tin/số liệu cụ thể của nhóm, chỉ cần thay bằng dữ liệu thực nghiệm.

## 1. Mục tiêu (theo PDF)

Mục tiêu của Lab 05 được chia thành 3 nhóm: **kiến thức nền tảng**, **triển khai hệ thống**, và **đánh giá kết quả**.

### 1.1. Kiến thức nền tảng

- **Tìm hiểu phần cứng Raspberry Pi 3:** nhận biết các thành phần chính và vai trò trong hệ thống IoT (CPU/RAM, lưu trữ thẻ SD, cổng CSI cho camera, USB, HDMI, GPIO, giao tiếp mạng). Từ đó xác định Raspberry Pi phù hợp cho vai trò **thiết bị biên (edge device)** thu thập dữ liệu cảm biến/hình ảnh.
- **Làm quen TensorFlow và NumPy:**
  - NumPy hỗ trợ thao tác ma trận/vector và tiền xử lý dữ liệu ảnh.
  - TensorFlow phục vụ chạy mô hình học sâu (FaceNet) để trích xuất đặc trưng khuôn mặt (embedding).
- **Hiểu quy trình phát triển ứng dụng AI:** nắm được chuỗi bước tối thiểu của một bài toán nhận diện: thu thập dữ liệu → tiền xử lý/chuẩn hóa → trích xuất đặc trưng → huấn luyện mô hình phân loại → đánh giá → triển khai chạy thử.

### 1.2. Mục tiêu triển khai hệ thống nhận diện (FaceNet)

- **Xây dựng pipeline Training và Recognition theo FaceNet:**
  - _Training:_ dùng mô hình FaceNet pretrained để trích xuất embedding từ ảnh đã align và huấn luyện classifier, sinh file mô hình phân loại (ví dụ `facemodel.pkl`).
  - _Recognition:_ khi nhận ảnh mới, thực hiện align → embedding → classifier để suy ra danh tính và điểm tin cậy.
- **Tiền xử lý dữ liệu bằng MTCNN/align:** chuẩn hóa ảnh khuôn mặt về cùng kích thước và vị trí tương đối (ví dụ 160×160) nhằm giảm sai lệch do góc chụp/vị trí mặt và tăng độ ổn định cho bước nhận diện.

### 1.3. Mục tiêu triển khai mô hình IoT (theo yêu cầu phần D trong PDF)

- **Xây dựng ứng dụng theo kiến trúc gồm Raspberry Pi 3 và PC/Laptop (edge server):**
  - Raspberry Pi thực hiện **thu ảnh** từ camera.
  - Raspberry Pi **gửi ảnh** qua Internet tới edge server (có thể theo HTTP/MQTT/RPC; triển khai mẫu dùng HTTP).
  - Edge server **nhận ảnh và nhận diện** bằng pipeline FaceNet.
  - Edge server **trả kết quả** về Raspberry Pi.
  - Raspberry Pi **hiển thị kết quả** nhận diện.

### 1.4. Mục tiêu đánh giá và yêu cầu đạt

- **Độ chính xác nhận diện:** kiểm tra độ chính xác nhận diện các thành viên trong nhóm và đảm bảo **> 60%** theo yêu cầu của PDF.
- **Đo thời gian nhận diện 1 ảnh (end-to-end):** xây dựng/viết hàm đo thời gian theo định nghĩa của PDF, tính từ thời điểm Raspberry Pi gửi ảnh lên edge server đến thời điểm Raspberry Pi nhận lại kết quả.
- **Tìm hiểu mã nguồn cung cấp:** đọc và mô tả vai trò các file chính, làm rõ các bước xử lý dữ liệu, luồng gọi hàm và dữ liệu vào/ra của từng thành phần (align, classifier training, Flask server, client Raspberry Pi).

---

## 2. Mô tả hệ thống & kiến trúc (theo Hình 1 trong PDF)

**Thành phần:**

- **Raspberry Pi 3 + Camera Raspberry Pi v2.1**: chụp ảnh từ camera, gửi ảnh lên server, nhận kết quả và hiển thị.
- **PC/Laptop (Edge server)**: nhận ảnh, chạy nhận diện FaceNet + classifier, trả kết quả về Raspberry Pi.
- **Giao tiếp qua Internet**: có thể dùng HTTP/MQTT/RPC; triển khai mẫu dùng **HTTP**.

**Luồng xử lý:**

1. Raspberry Pi chụp ảnh từ camera → 2) gửi ảnh lên edge server → 3) edge server nhận diện → 4) edge server trả kết quả → 5) Raspberry Pi hiển thị.

**Nguyên lý nhận diện (tóm tắt khoa học):**

- **MTCNN/align:** phát hiện khuôn mặt và căn chỉnh (align) để chuẩn hóa đầu vào.
- **FaceNet embedding:** biến đổi ảnh mặt đã chuẩn hóa thành vector đặc trưng $\mathbf{z} \in \mathbb{R}^d$.
- **Classifier:** học ranh giới phân lớp trên embedding để suy ra danh tính (tên lớp) và điểm tin cậy.

**Dữ liệu trao đổi Pi ↔ Server (mức khái niệm):**

- Request từ Pi: ảnh (thường dạng JPEG/PNG) + metadata (thời gian gửi, kích thước, ...).
- Response từ server: nhãn dự đoán (tên người/unknown) + score (xác suất/độ tin cậy) + (tuỳ chọn) thời gian xử lý.

---

## 3. Môi trường thực nghiệm

### 3.1. Phần cứng

- Raspberry Pi 3: [điền]
- Camera Raspberry Pi v2.1: [điền]
- PC/Laptop (edge server): [điền]
- Kết nối mạng (Wi‑Fi/LAN): [điền]

**Cấu hình mạng (khuyến nghị ghi rõ để tái lập thí nghiệm):**

- IP Raspberry Pi: [điền]
- IP edge server: [điền]
- Port dịch vụ nhận diện (Flask): [điền]

### 3.2. Phần mềm

- Raspberry Pi OS: [điền]
- Python: [điền]
- Các thư viện theo `requirements.txt`
- VNC để truy cập Raspberry Pi: [điền]

**Ghi chú:** sử dụng đúng phiên bản thư viện theo `requirements.txt` để đảm bảo tương thích mô hình và mã nguồn cung cấp trong tài nguyên bài lab.

---

## 4. Dataset (theo Bước 2 trong PDF)

### 4.1. Cấu trúc thư mục

Tạo dataset theo dạng mỗi người một thư mục lớp:

```
Dataset/
  raw/
    NguyenVanA/
    NguyenVanB/
```

- Số lớp (số người): [điền]
- Số ảnh mỗi người (ước lượng): [điền]

**Quy ước thu thập ảnh (ghi ngắn gọn):**

- Ảnh chụp chính diện và hơi nghiêng; đa dạng điều kiện sáng; hạn chế che khuất.
- Mỗi người nên có đủ ảnh để classifier học được đặc trưng ổn định.

---

## 5. Quy trình thực hành (theo mục C trong PDF)

> Lưu ý: các bước/command dưới đây được giữ đúng theo nội dung PDF.

### Bước 1 — Tải tài nguyên bài lab

- Tải bộ tài nguyên theo link Google Drive trong PDF và giải nén vào thư mục làm việc.

**Sản phẩm sau bước này:** bộ mã nguồn + thư mục mô hình pretrained (ví dụ `.pb`) + cấu trúc dữ liệu mẫu.

### Bước 2 — Tạo dataset

- Tạo `Dataset/raw/<TenNguoi>/...` và thêm ảnh cho từng thành viên.

**Sản phẩm sau bước này:** thư mục `Dataset/raw` chứa ảnh theo từng lớp người.

### Bước 3 — Cài thư viện

Chạy:

```bash
pip install -r requirements.txt
```

**Giải thích:** bước này đảm bảo môi trường có đủ thư viện cho (i) phát hiện/align khuôn mặt, (ii) chạy mô hình FaceNet, và (iii) chạy web server Flask.

### Bước 4 — Tiền xử lý dữ liệu (align)

Chạy:

```bash
python src/align_dataset_mtcnn.py Dataset/raw Dataset/processed --image_size 160 -margin 32 --random_order --gpu_memory_fraction 0.25
```

**Kết quả mong đợi:** tạo thư mục `Dataset/processed` chứa ảnh khuôn mặt đã phát hiện và căn chỉnh.

**Giải thích các tham số chính:**

- `--image_size 160`: chuẩn hóa kích thước ảnh mặt về 160×160 (phù hợp đầu vào FaceNet).
- `-margin 32`: mở rộng vùng crop quanh mặt để giữ ngữ cảnh vừa đủ.
- `--random_order`: xáo trộn thứ tự ảnh khi xử lý.
- `--gpu_memory_fraction 0.25`: giới hạn bộ nhớ GPU (nếu có) để tránh chiếm dụng toàn bộ.

### Bước 5 — Huấn luyện classifier

Chạy:

```bash
python src/classifier.py TRAIN Dataset/processed Models/20180402-114759.pb Models/facemodel.pkl --batch_size 1000
```

**Kết quả mong đợi:** tạo file `Models/facemodel.pkl` (mô hình classifier và nhãn lớp).

**Giải thích (ngắn gọn):**

- Mô hình FaceNet pretrained (`Models/20180402-114759.pb`) được dùng để trích xuất embedding cho từng ảnh.
- Classifier được huấn luyện trên embedding để phân biệt các thành viên (các lớp người).

### Bước 6 — Chạy Flask server (edge server)

Chạy:

```bash
python src/face_rec_flask.py
```

**Mô tả chức năng:**

- Khởi tạo mô hình FaceNet + nạp `facemodel.pkl`.
- Cung cấp API nhận ảnh từ Raspberry Pi, chạy nhận diện và trả kết quả.

### Bước 7 — Chạy demo nhận diện (Raspberry Pi)

Chạy:

```bash
python client.py
```

**Mô tả chức năng:**

- Chụp ảnh từ camera, gửi ảnh lên edge server.
- Nhận kết quả dự đoán và hiển thị (tên/score).

---

## 6. Kết quả & đánh giá (theo yêu cầu mục D trong PDF)

### 6.1. Độ chính xác nhận diện (yêu cầu > 60%)

**Thiết lập đánh giá:**

- Tiêu chí: Accuracy trên ảnh kiểm thử của các thành viên trong nhóm.
- Bộ ảnh test: sử dụng **41 ảnh** trong `Dataset/raw` (4 lớp: `khanh`, `quang`, `vanhau`, `vantoan`). Với mỗi ảnh, client gửi ảnh lên endpoint `POST /recog` và so sánh nhãn trả về với tên thư mục lớp (ground-truth).

**Cách tính:** Accuracy = (Số dự đoán đúng / Tổng số mẫu kiểm thử) × 100%.

| Người    | Số ảnh test |   Đúng |   Sai | Accuracy (%) |
| -------- | ----------: | -----: | ----: | -----------: |
| khanh    |          18 |     18 |     0 |      100.00% |
| quang    |          15 |      8 |     7 |       53.33% |
| vanhau   |           4 |      4 |     0 |      100.00% |
| vantoan  |           4 |      3 |     1 |       75.00% |
| **Tổng** |      **41** | **33** | **8** |   **80.49%** |

**Kết luận:** Accuracy tổng = **80.49%** (đạt yêu cầu PDF: > 60%).

**Nhận xét ngắn:** lớp `quang` có độ chính xác thấp hơn do điều kiện ảnh (góc mặt/ánh sáng/độ rõ) và sai số ở bước phát hiện–crop khuôn mặt trước khi trích embedding. Các lớp còn lại có độ chính xác cao khi khuôn mặt được phát hiện rõ và ảnh đủ sắc nét.

### 6.2. Đo thời gian nhận diện 1 ảnh (end-to-end)

**Định nghĩa theo PDF:** thời gian tính từ lúc Raspberry Pi gửi ảnh đến edge server cho đến lúc nhận kết quả trả về.

**Phương pháp đo:** đo thời gian tại phía client bằng cách ghi nhận timestamp trước khi gửi HTTP request và sau khi nhận response.

Gọi:

- `t_send`: thời điểm bắt đầu gửi ảnh
- `t_recv`: thời điểm nhận kết quả

Khi đó độ trễ cho một ảnh: `T = t_recv - t_send`.

**Thiết lập đo:** thực hiện gửi lần lượt 41 ảnh trong `Dataset/raw` đến endpoint `POST /recog`, ghi nhận $T$ cho từng request và thống kê.

**Kết quả thống kê (ms):**

- Số lần đo $N = 41$
- Trung bình: **181.42 ms**
- Độ lệch chuẩn: **56.00 ms**
- Trung vị (median): **197.50 ms**
- Min/Max: **60.87 ms / 247.87 ms**

**Nhận xét ngắn:** độ trễ phụ thuộc chủ yếu vào (i) thời gian phát hiện khuôn mặt (MTCNN), (ii) thời gian suy luận embedding của FaceNet, và (iii) kích thước ảnh + độ ổn định đường truyền HTTP.

---

## 7. Tìm hiểu mã nguồn (yêu cầu 4 trong PDF)

Phần này trình bày chi tiết các **quy trình**, **bước xử lý**, và **dữ liệu vào/ra** trong các file chính của mã nguồn cung cấp. Toàn bộ pipeline có thể mô tả gọn thành:

**Align (MTCNN) → Embedding (FaceNet) → Classify (SVM) → Trả kết quả (Flask/HTTP)**.

### 7.1. Tiền xử lý dữ liệu: `src/align_dataset_mtcnn.py`

**Mục đích:** chuyển dữ liệu ảnh thô trong `Dataset/raw` thành ảnh khuôn mặt đã phát hiện/cắt/căn chỉnh trong `Dataset/processed` để dùng cho bước huấn luyện classifier.

**Đầu vào:**

- `input_dir`: thư mục ảnh thô theo lớp người (mỗi thư mục là một lớp).
- `output_dir`: thư mục xuất ảnh đã align.
- Tham số chính: `--image_size`, `--margin`, `--random_order`, `--detect_multiple_faces`.

**Đầu ra:**

- Ảnh đã chuẩn hóa kích thước (theo `--image_size`) lưu dạng `.png` vào `output_dir/<class_name>/...`.
- File log bounding box `bounding_boxes_XXXXX.txt` lưu thông tin crop (hoặc dòng lỗi nếu không align được).

**Quy trình xử lý chính (theo luồng code):**

1. Khởi tạo `output_dir`, ghi thông tin phiên bản/revision bằng `facenet.store_revision_info(...)`.
2. Đọc dataset bằng `facenet.get_dataset(args.input_dir)` → trả về danh sách lớp, mỗi lớp có danh sách đường dẫn ảnh.
3. Khởi tạo mạng MTCNN: tạo graph TensorFlow và gọi `align.detect_face.create_mtcnn(sess, None)` để lấy các mạng `pnet, rnet, onet`.
4. Với từng ảnh:

- Đọc ảnh (dùng `imageio.imread`). Nếu ảnh grayscale thì chuyển sang RGB (`facenet.to_rgb`).
- Phát hiện mặt: `align.detect_face.detect_face(img, minsize, pnet, rnet, onet, threshold, factor)`.
- Nếu có nhiều mặt và `detect_multiple_faces=False`, chọn 1 mặt “tối ưu” dựa trên **diện tích bounding box** và độ **gần tâm ảnh**.
- Tính bounding box có `margin`, crop vùng mặt, resize về `image_size × image_size` (BILINEAR), lưu ra file `.png`.
- Ghi bounding box tương ứng vào file `bounding_boxes_XXXXX.txt`.

**Ý nghĩa:** bước align giúp chuẩn hóa đầu vào (vị trí/kích thước khuôn mặt), giảm sai lệch do nền, khoảng cách camera, góc quay… từ đó làm classifier ổn định hơn.

### 7.2. Huấn luyện/kiểm thử classifier: `src/classifier.py`

**Mục đích:** dùng mô hình FaceNet pretrained để trích xuất embedding cho toàn bộ ảnh đã align, sau đó:

- `TRAIN`: huấn luyện classifier SVM và lưu ra `facemodel.pkl`.
- `CLASSIFY`: nạp `facemodel.pkl` và đánh giá/hiển thị kết quả dự đoán, tính accuracy.

**Đầu vào (theo tham số dòng lệnh):**

- `mode`: `TRAIN` hoặc `CLASSIFY`.
- `data_dir`: thư mục ảnh đã align (ví dụ `Dataset/processed`).
- `model`: mô hình FaceNet (đường dẫn `.pb` hoặc thư mục checkpoint).
- `classifier_filename`: đường dẫn output/input `.pkl`.
- `--batch_size`, `--image_size`, `--use_split_dataset` (tuỳ chọn chia train/test).

**Đầu ra:**

- Với `TRAIN`: file pickle chứa `(model, class_names)` được lưu tại `classifier_filename`.
- Với `CLASSIFY`: in ra dự đoán cho từng ảnh và accuracy trung bình.

**Quy trình xử lý chính (theo luồng code):**

1. Đọc dataset: `facenet.get_dataset(data_dir)` và tạo danh sách `(paths, labels)` bằng `facenet.get_image_paths_and_labels(...)`.
2. Nạp FaceNet: `facenet.load_model(args.model)`; lấy các tensor:

- `input:0` (ảnh đầu vào),
- `embeddings:0` (vector đặc trưng),
- `phase_train:0` (cờ train/inference).

3. Tính embedding theo mini-batch:

- Load ảnh bằng `facenet.load_data(paths_batch, False, False, args.image_size)`.
- Chạy forward: `sess.run(embeddings, feed_dict=...)` và ghi vào `emb_array`.

4. Với `TRAIN`:

- Khởi tạo `SVC(kernel='linear', probability=True)`.
- Huấn luyện `model.fit(emb_array, labels)`.
- Tạo `class_names` từ tên thư mục lớp và lưu pickle `(model, class_names)`.

5. Với `CLASSIFY`:

- Nạp pickle, chạy `predict_proba`, lấy lớp tốt nhất bằng `argmax`.
- Tính accuracy: `np.mean(np.equal(best_class_indices, labels))`.

**Ý nghĩa:** classifier SVM không học trực tiếp từ pixel, mà học trên embedding của FaceNet nên thường cần ít dữ liệu hơn và huấn luyện nhanh hơn.

### 7.3. Edge server nhận diện: `src/face_rec_flask.py`

**Mục đích:** tạo Flask server cung cấp endpoint nhận ảnh (HTTP POST), chạy pipeline nhận diện và trả kết quả về client.

**Tải mô hình (xảy ra khi khởi động server):**

1. Nạp classifier từ `Models/facemodel.pkl`:

- `model`: SVM đã huấn luyện
- `class_names`: danh sách tên lớp

2. Tạo session TensorFlow, nạp FaceNet pretrained từ `./Models/20180402-114759.pb`.
3. Lấy các tensor đúng tên trong graph: `input:0`, `embeddings:0`, `phase_train:0`.
4. Khởi tạo MTCNN bằng `align.detect_face.create_mtcnn(sess, "src/align")`.

**Endpoint chính:** `POST /recog`

- **Input:** form field `image` chứa **Base64-encoded bytes** của ảnh (client gửi kèm `w`, `h` nhưng server hiện tại không sử dụng).
- **Các bước xử lý trong endpoint:**
  1. Ghi nhận `start_time` để đo latency xử lý phía server.
  2. Giải mã base64 → bytes → `cv2.imdecode` thành ảnh `frame` (mảng NumPy).
  3. Nếu `frame` giải mã thất bại (None) thì trả về `Unknown`.
  4. Chuyển đổi không gian màu **BGR → RGB** (do OpenCV decode mặc định là BGR) để phù hợp với pipeline MTCNN/FaceNet.
  5. Phát hiện mặt trên `frame` bằng MTCNN (`detect_face`).
  6. Với mỗi bounding box:
  - Crop vùng mặt, resize về `160×160` (`INPUT_IMAGE_SIZE`).
  - Chuẩn hóa bằng `facenet.prewhiten`.
  - Reshape về batch (1,160,160,3), chạy FaceNet để lấy `emb_array`.
  - Chạy `model.predict_proba(emb_array)` để lấy xác suất theo từng lớp.
  - Lấy lớp tốt nhất và xác suất tương ứng.
  - Áp dụng ngưỡng: nếu `prob > 0.3` → trả tên lớp, ngược lại trả `Unknown`.
  5. Tính latency (ms) và log ra console dạng:
     `Identity | Prob | Latency`.
- **Output:** server trả về **chuỗi tên** (ví dụ tên thành viên hoặc `Unknown`).

**Các tham số nhận diện quan trọng trong file:**

- `MINSIZE = 20`, `THRESHOLD = [0.6, 0.7, 0.7]`, `FACTOR = 0.709` (tham số MTCNN).
- `INPUT_IMAGE_SIZE = 160` (kích thước ảnh đưa vào FaceNet).
- Ngưỡng nhận/không nhận: `prob > 0.3`.

### 7.4. Client gửi ảnh: `client.py`

**Mục đích:** gửi ảnh lên endpoint `/recog` để demo quá trình nhận diện.

**Quy trình chính:**

1. Đọc ảnh từ `image_path` và encode base64 (hàm `encode_image_to_base64`).
2. Tạo request `POST` đến `url` (mặc định trong file là `http://127.0.0.1:8000/recog`).
3. Gửi form data:

- `image`: chuỗi base64
- `w`, `h`: kích thước (đặt 160 để khớp đầu vào FaceNet; hiện server không dùng hai giá trị này).

4. In response từ server ra terminal.

**Đầu vào/đầu ra:**

- Input: đường dẫn ảnh (file `.jpg/.png`).
- Output: chuỗi danh tính do server trả về.

### 7.5. Tổng hợp luồng end-to-end theo mã nguồn

1. Chuẩn bị dữ liệu: `Dataset/raw/<TenNguoi>/...`
2. Align dữ liệu: chạy `align_dataset_mtcnn.py` → sinh `Dataset/processed/<TenNguoi>/...`
3. Huấn luyện classifier: chạy `classifier.py TRAIN` → sinh `Models/facemodel.pkl`
4. Chạy server: `face_rec_flask.py` nạp `.pb` + `.pkl`, mở API `/recog`
5. Client gửi ảnh: `client.py` encode base64 và gửi HTTP POST, nhận về chuỗi tên.

Phần đo độ trễ end-to-end theo yêu cầu PDF được thực hiện bằng cách đo thời gian ở phía client (từ lúc gửi request đến lúc nhận response), kết hợp với log latency phía server để tham khảo thời gian xử lý nội bộ.

---

## 8. Kết luận

- Báo cáo đã trình bày đầy đủ mô hình IoT theo yêu cầu: Raspberry Pi thu ảnh → edge server nhận diện → trả kết quả.
- Độ chính xác nhận diện thành viên nhóm: **[ ]%** (đối chiếu yêu cầu PDF: > 60%).
- Độ trễ end-to-end trung bình: **[ ] ms** theo định nghĩa trong PDF.

**Hướng cải thiện (ngắn gọn):** tăng số ảnh huấn luyện, cân bằng điều kiện ánh sáng, ổn định mạng truyền, và kiểm tra chất lượng ảnh sau align.

---

## 9. Yêu cầu nộp bài (theo PDF)

- Giải thích kết quả đạt được (mục 6).
- Kèm ảnh chụp màn hình minh họa theo yêu cầu môn học (có thể chọn ảnh đại diện, tối giản).
- Cung cấp video demo ứng dụng.
- Nộp báo cáo theo mẫu + toàn bộ mã nguồn.
- Đặt đúng tên file nén theo yêu cầu (ví dụ `Nhom1-Lab05_20520001_20520002`).

---

## Tài liệu tham khảo

1. Tài liệu Lab 05: “Xây dựng ứng dụng AI cơ bản trong mô hình IoT” (UIT).
2. FaceNet: A Unified Embedding for Face Recognition and Clustering (Schroff et al.).

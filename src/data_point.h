
template <typename T> class DataPoint {
private:
  int timestamp;
  T value;

public:
  T get_value() { return value; }
  DataPoint(int timestamp, T value) {
    this->value = value;
    this->timestamp = timestamp;
  }
};

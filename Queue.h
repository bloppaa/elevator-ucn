#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
class Queue
{
private:
  T *data;
  int capacity;
  int front;
  int rear;
  int count;

public:
  Queue(int size)
  {
    capacity = size;
    data = new T[capacity];
    front = 0;
    rear = -1;
    count = 0;
  }

  ~Queue()
  {
    delete[] data;
  }

  void enqueue(T item)
  {
    if (isFull() || contains(item))
    {
      return;
    }
    rear = (rear + 1) % capacity;
    data[rear] = item;
    count++;
  }

  T dequeue()
  {
    if (isEmpty())
    {
      return T();
    }
    T item = data[front];
    front = (front + 1) % capacity;
    count--;
    return item;
  }

  bool contains(T item)
  {
    for (int i = 0; i < count; i++)
    {
      if (data[(front + i) % capacity] == item)
      {
        return true;
      }
    }
    return false;
  }

  void print()
  {
    if (isEmpty())
    {
      Serial.println("Queue is empty.");
      return;
    }
    Serial.print("Queue: [");
    for (int i = 0; i < count; i++)
    {
      Serial.print(data[(front + i) % capacity]);
      if (i < count - 1)
      {
        Serial.print(", ");
      }
    }
    Serial.println("]");
  }

  bool isFull() const
  {
    return count == capacity;
  }

  bool isEmpty() const
  {
    return count == 0;
  }
};

#endif

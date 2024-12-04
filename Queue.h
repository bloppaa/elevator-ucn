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
        if (isFull())
        {
            return; // Queue is full, ignore enqueue
        }
        rear = (rear + 1) % capacity;
        data[rear] = item;
        count++;
    }

    T dequeue()
    {
        if (isEmpty())
        {
            return T(); // Return default value if empty
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

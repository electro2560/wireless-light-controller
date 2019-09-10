class Group {
  private:
    const int channels;
    const int startingChannel;
  public:
    Group(const int channels, const int startingChannel)
      : channels(channels), startingChannel(startingChannel) {};
    virtual ~Group() {};
    virtual void update() = 0;

};

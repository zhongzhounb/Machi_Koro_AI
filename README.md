# Machi Koro Qt (骰子街 Qt 版) 🎲

## 简介

本项目是基于 C++ 和 Qt 框架实现的桌面版《骰子街》(Machi Koro) 游戏，但你如果能看到这里，说明你的成分是：

- 会玩骰子街但正版太贵了
- 不会玩骰子街但想用Qt写游戏
- 通过各种机器人转发在各种平台的项目点进来了



![1](C:\Users\Administrator\Desktop\Machi_Koro_AI\readmeimg\1.png)

这个是我希望做的图啦，但是我现在做成的是这样：

![2](C:\Users\Administrator\Desktop\Machi_Koro_AI\readmeimg\2.png)



项目还未完工，后端已经大体完成，现在在苦恼做前端UI动画。。。

但先别走！你可以先关注我，以后就是老粉啦~

## 进度列表

- [x] ### 需求分析 6.1~6.4

  “做一款属于自己的游戏一直是我的梦想。”

  “我特别喜欢玩骰子街，线下没人和我玩，线上Steam卖太贵了。”

  “Qt怎么没什么人用来写游戏啊！”

  基于以上三点，所以就有了我尝试用Qt写一个骰子街的想法。。。

- [x] ### 可行性分析 6.4~6.7

  做一个游戏前，最重要的就是得考虑游戏逻辑实现难度、AI决策实现难度以及UI及动画实现难度。

  #### 游戏逻辑实现难度

  游戏逻辑实现难度无非就三种操作、抛骰子操作、卡牌运作、买卡操作，其中卡牌运作大多是金币增加和金币减少，偶尔有少量换卡的操作，最难点在于卡牌的执行顺序，也就是说卡牌运作方式顺序不同，但是基础操作相同，为了不写死顺序，使得程序有扩展性，可以运用命令模式实现，即将各种操作转化为命令，放入命令队列里顺序执行。

  #### AI决策实现难度

  做单机游戏肯定要有AI决策的，这款美式桌游优点就是随机性大于决策性，在AI方面不需要复杂设计，甚至就random选择运气好人机都能赢，难一点就设计成一直买贵的卡（怕别人偷），再难点设计无法就是算买各种卡的下一轮收益期望（目标实现），遇到算期望的事AI比真人就好用多了。

  不只买卡，其他操作比如选则扔一个还是两个、是否重抛，还是+2啥的，都可以算期望实现利益最大化，也是非常简单的算法。

  #### UI及动画实现难度

  UI已经有现成的UI了，扫描后再扣下来能省非常多的事，剩下的图随便找几个生图软件生成就行，不在美工上花太多功夫（虽然我也学了2年美术）。

  动画是最难的了，因为之前没学过Qt，不知道如何做动画，但没关系，走一步算一步吧，动画有无都行，只影响观感不影响游戏体验🤭。

  

  鉴于AI、UI都很容易设计，决心将这款游戏做大做强，完成我想做一款游戏的心愿（以后上班估计没这么闲了）

  

- [x] ### MVC架构设计 6.8~6.15

  

- [x] ### 命令模式设计 6.16~6.22 （待完善）

  由于前面提到卡牌有很多不同的顺序，所以得设计命令模式，当某张卡被激活时，应该生成一个或多个命令，生成在controller的命令队列中，等待被处理。处理机制应该有controller，流程图为：

  ```mermaid
  flowchart TD
      A[processNextCommandInQueue启动] --> B{当前命令是否需要用户输入}
      B --> C{当前玩家是否是AI}
      B --> G{执行命令 commandexecute}
  
      C --> D[AI计算并设置 userChoice]
      D --> G
  
      C --> E[向UI发送请求输入信号]
      E --> F[等待玩家输入后返回]
      F --> A
  
      G --> H{是否在回放模式}
      H --> I[记录命令到日志]
      I --> J{是否需要UI动画}
      H --> J
  
      J --> K[向UI发送动画信号]
      K --> L[继续处理下一个命令]
      J --> L
  
      L --> M{命令队列是否为空}
      M --> A
      M --> N[切换到下一个游戏阶段或结束回合]
      N --> A
  ```

  由于还需要前端交互，目前只做了初步构想，所以待完善。

- [x] ### 命令顺序设计 6.16~6.19

  根据游戏基本顺序、卡牌执行顺序（先红再蓝绿再紫）、同红色优先执行离投掷者最近玩家、同颜色卡执行顺序（先扣钱（如借贷公司）、再获得钱、再combo，最后特殊技能（如拆迁公司、搬家公司）等，设计的命令顺序如下：

  ```mermaid
  ---
  config:
    kanban:
      ticketBaseUrl: 'https://mermaidchart.atlassian.net/browse/#TICKET#'
  ---
  kanban
    Todo
      [Create Documentation]
      docs[Create Blog about the new diagram]
    [In progress]
      id6[Create renderer so that it works in all cases. We also add some extra text here for testing purposes. And some more just for the extra flare.]
    id9[Ready for deploy]
      id8[Design grammar]@{ assigned: 'knsv' }
    id10[Ready for test]
      id4[Create parsing tests]@{ ticket: MC-2038, assigned: 'K.Sveidqvist', priority: 'High' }
      id66[last item]@{ priority: 'Very Low', assigned: 'knsv' }
    id11[Done]
      id5[define getData]
      id2[Title of diagram is more than 100 chars when user duplicates diagram with 100 char]@{ ticket: MC-2036, priority: 'Very High'}
      id3[Update DB function]@{ ticket: MC-2037, assigned: knsv, priority: 'High' }
  
    id12[Can't reproduce]
      id3[Weird flickering in Firefox]
  
  
  ```

  因为有些命令不是通过卡产生，比如抛骰子、买卡的这种没回合都得操作，所以当新的玩家开始游戏时，得固定放上某些操作，不如封装成一个命令，用以生成某些命令。

  按照这个规则，能将所有操作都转化成命令，则设计的命令创建顺序为：

  ```mermaid
  flowchart TD
      A[回合开始] --> B[抛骰子阶段]
      A --> C[激活地标阶段]
      A --> E[激活卡阶段]
      A --> F[建造阶段]
  
      C --> G[港口]
      C --> H[游乐园]
      C --> D[其他地标...]
    
      E --> J[麦田]
      E --> K[面包房]
      E --> L[其他卡...]
  
  
  ```

  但问题来了，我希望在设计UI时：如果是卡牌的命令，在即将执行的时候保持”闪烁“状态，那么就得设计某张卡牌什么时候该亮，什么时候该暗。比如我抛骰子动画之前，港口是不闪烁的（因为你此时不知道你抛的是什么），但是你抛到了10以上，在选择是否+2的时候，港口闪烁，而你如果抛到10以下，港口一直不闪烁。

  所以有两种方案，要不设计一个由命令发出的信号提示卡牌闪烁和取消闪烁状态，要不直接设计二级命令产生三级命令对象，队列中存在某个卡牌命令就直接保持闪烁。前者好处是创建和闪烁职责分离使得代码好理解，后者好处是不用考虑闪烁问题代码简单除暴。我选择后者，因为你会发现，你让一个卡闪烁/取消闪烁，正是创建/取消了他的命令，也就是说闪烁和命令是强绑定的，于是就有了下面的命令创建顺序：

  ```mermaid
  flowchart TD
      A[回合开始] --> B[抛骰子阶段]
      A --> |有广播塔| C[广播塔]
      A --> D[激活卡阶段]
      A --> E[市政府]
      A --> F[建造阶段]
  
      B --> |有港口且骰子>=10| G[港口]
  
      C --> |有港口且骰子>=10| G
      C -.-> |有港口且骰子<10| G
  
      D --> |有游乐园| H[游乐园]
      D --> I[获得金币等卡牌命令...]
  
      E --> J[机场]
  
      F --> |有科技公司| K[科技公司]
      F -.-> |买了东西| J
  ```

  其中，实线为创建，虚线为删除，虽然理解起来很复杂，但是这大大简化了代码量。

- [x] ### 工厂模式设计 6.20~6.21

  工厂模式就类似方法和类名映射了，这种将文件树发给AI就能生成工厂，不必花费太多精力。

  ```mermaid
  classDiagram
      direction LR
  
      class Card {
          <<abstract>>
          // 抽象卡牌基类，定义所有卡牌的通用接口
           +QString name()
           +int cost()
           +CardColor color()
           +QList<int> activationDiceRolls()
          
      }
  
      class AppleOrchard {
          // 具体蓝色卡牌示例
          +AppleOrchard(QObject* parent = nullptr)
      }
      class Bakery {
          // 具体绿色卡牌示例
          +Bakery(QObject* parent = nullptr)
      }
      class Airport {
          // 具体地标卡牌示例
          +Airport(QObject* parent = nullptr)
      }
  
      Card <|-- AppleOrchard
      Card <|-- Bakery
      Card <|-- Airport
  
      class CardFactory {
          <<Singleton>>
          -QMap<QString, CardType> m_cardNameToTypeMap
          -CardFactory() // 私有构造函数，实现单例
          -void initializeCardMap()
          -Card* createCardInternal(CardType type, QObject* parent)
          +static CardFactory& instance()
          +Card* createCard(const QString& cardName, QObject* parent = nullptr)
          +QList<Card*> createCards(const QString& cardName, int count, QObject* parent = nullptr)
      }
  
      CardFactory ..> Card : 创建 (creates)
  ```

  

- [x] ### 卡牌图片资源准备 6.27~6.29

  我扫描的图片除了卡上的光线不均会影响观感，色差也是非常头痛的问题：

  ![3](C:\Users\Administrator\Desktop\Machi_Koro_AI\readmeimg\3.png)

  所以毅然决然的自己重画一张，背景好画，可以用PS但没必要，用![4](C:\Users\Administrator\Desktop\Machi_Koro_AI\readmeimg\4.png)画的，只要画几个不规则图片矩形排布就行，然后取卡牌上的某点颜色同意配色。

  最难的就是建筑icon，因为icon之间色彩差异大，色差不明显，决定采用直接抠图，背景透明。抠图可以用PS但也没必要，直接网上随便搜个AI抠图再人工修改以下简单又省事，我用![5](C:\Users\Administrator\Desktop\Machi_Koro_AI\readmeimg\5.png)实现的：

  ![6](C:\Users\Administrator\Desktop\Machi_Koro_AI\readmeimg\6.png)

  剩下的文字后端肯定得记录，就直接在前端生成了。

  

- [ ] ### 卡牌UI设计及自适应 6.30~今

因为卡牌是核心，其他都能不显示就是卡牌不能不显示，所以解决卡牌UI，其他就没有不能解决的！

因为卡牌不是整合的图片，被拆分了，增加了扩展性减少内存的同时，自适应就非常麻烦了，你得要求：

- 窗口大小能更改卡牌的大小，字体随之改变
- 卡牌不能被拉伸（即不能更改卡牌的长宽比）
- 在界面显示完整且布局各中最大化
- 文字和图片保持相对位置保持不变

头疼，目前在搞这块，Qt这方面没有封装，谁说搞前端容易的QAQ

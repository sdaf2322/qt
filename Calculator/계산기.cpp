#include "widget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>

Widget::Widget(QWidget *parent)
    : QWidget(parent), m_isFirst(true) {        // 부모 생성자 정의, private 변수 초기화
    m_label = new QLabel("0", this);			// 레이블 객체의 생성, 0은 레이블에 표시할 글자
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);	// 라벨에 표시될 글자 정렬. 오른쪽 정렬 + 가운데 맞춤. 이진수로 되어있기 때문에 비트연산자 사용
    m_label->setFrameShape(QFrame::Box); 		// 외각의 프레임 설정
    m_label->setMaximumHeight(30);              // 라벨의 최대크기 크기 설정. 라벨을 드래그 했을 때 30이상 안늘어남

    // 버튼에 표시될 문자열
    const char ButtonChar[16][2] = {            // 버튼에 표시할 글자를 배열로 나타냄. ""로 하면 string, string은 공백을 포함하기 때문에 크기를 고려해서 [16][2]로 나타냄.
                                                // QString으로 받고 1차원 배열로 만들어도 됨
        "7", "8", "9", "/",                     // QPushButton 생성자의 첫번째 인자로 문자열만 사용 가능
        "4", "5", "6", "x",
        "1", "2", "3", "-",
        "0", "C", "=", "+"
    };

    QGridLayout *gridLayout = new QGridLayout( );   // 그리드레이아웃 객체 생성, 각각의 위젯을 열과 행에 맞춰 배치
    buttonGroup = new QButtonGroup(this);           // 버튼그룹 객체 생성, 버튼 위젯을 배치할 컨테이너 제공
    connect(buttonGroup, SIGNAL(idClicked(int)), SLOT(click(int)));     // 버튼을 눌렀을 때 해당 버튼의 ID에 따른 slot 처리
    for(int y = 0; y < WITDH; ++y) {		// 4 x 4의 2차원으로 배치, 행 중심으로 읽는게 속도가 빠르기 때문에 y를 먼저 사용
        for(int x = 0; x < WITDH; ++x) {
            int n = x+y*WITDH;		 	// 2차원 공간을 1차원으로 변환, 버튼을 좌표에 맞게 배치하기 위해 사용
            buttonGroup->addButton(new QPushButton(ButtonChar[n], this), n);    // 각각의 버튼들을 그룹화
            gridLayout->addWidget(buttonGroup->button(n), y, x); // 각각의 버튼을 행렬에 맞게 배치
        }
    }

    // Clear 버튼을 위한 시그널/슬롯 연결 : 람다 함수
    connect(buttonGroup->button(13), &QPushButton::clicked, this, [=]( ){ m_label->setText("0"); }); // c눌렀을 때 초기화

    // 계산을 위한 코드 : 람다 함수
    connect(buttonGroup->button(14), &QPushButton::clicked, this, [this]( ){
        qreal result = 0;
        m_isFirst = true;               // 새로운 값으로 입력
        if(m_op == "+")           // 덧셈
            result = m_num1.toDouble( ) + m_label->text( ).toDouble( ); // QString을 double로
        else if(m_op == "-")      // 뺄셈
            result = m_num1.toDouble( ) - m_label->text( ).toDouble( );
        else if(m_op == "x")      // 곱셈
            result = m_num1.toDouble( ) * m_label->text( ).toDouble( );
        else if(m_op == "/")      // 나눗셈 : 에러 처리 필요
            if(m_label->text( ).toDouble( )) {
                result = m_num1.toDouble( ) / m_label->text( ).toDouble( );
            } else {
                m_label->setText("Error : Cannot Divide by Zero");
                return;
            }
        m_label->setText(QString::number(result));	// 숫자를 QString으로 변경
    });

    setWindowTitle("Calculator");		// 윈도우의 타이틀바 설정

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setContentsMargins(6, 6, 6, 6);
    vBoxLayout->addWidget(m_label);
    vBoxLayout->addLayout(gridLayout);
    setLayout(vBoxLayout);
}

Widget::~Widget( ) {			// 생성자에서 할당한 객체들의 정리 작업
    delete m_label;
    Q_FOREACH(auto b, buttonGroup->buttons()) delete b; 	// QButtonGroup 클래스 안의 객체 삭제
}

void Widget::setNum( ) {		// 숫자 버튼이 눌려졌을 때의 처리
    // 시그널을 호출한 객체는 QObject::sender( ) 메소드로 받아올 수 있다.
    QPushButton* button = dynamic_cast<QPushButton*>(sender( ));
    QString bStr;
    if(button != nullptr) bStr = button->text( );	// 버튼의 문자열
    if(m_label != nullptr) {
        QString lStr = m_label->text( );		// 레이블의 문자열
#if 1
        // 레이블의 문자가 0이면 버튼의 문자를 레이블에 그대로 설정
        // 0이 아니면 레이블의 문자열 + 버튼의 문자열을 레이블에 설정
        m_label->setText((lStr =="0"|m_isFirst)?bStr:lStr+bStr);
#else
        m_label->setText(QString::number(lStr.toDouble( )*10 +bStr.toDouble( )));
#endif
        m_isFirst = false;          // 기존의 값 뒤에 연결해서 입력
    }
}

void Widget::setOp( )
{
    QPushButton* button = dynamic_cast<QPushButton*>(sender( ));
    if(button != nullptr) m_op = button->text( );	// 버튼의 문자열이 연산자
    if(m_label != nullptr) {
        m_num1 = m_label->text( );	// 현재 레이블의 문자열을 멤버 변수로 설정
        m_isFirst = true;           // m_label->setText("0");
    }
}

void Widget::click(int id)
{
    QPushButton* button = (QPushButton*)((QButtonGroup*)sender( ))->button(id);
    QString bStr;
    if(button != nullptr) bStr = button->text( );	// 버튼의 문자열
    switch(id) {
    case 0: case 1: case 2:
    case 4: case 5: case 6:
    case 8: case 9: case 10:
    case 12:
        if(m_label != nullptr) {
            QString lStr = m_label->text( );		// 레이블의 문자열
            // 레이블의 문자가 0이면 버튼의 문자를 레이블에 그대로 설정
            // 0이 아니면 레이블의 문자열 + 버튼의 문자열을 레이블에 설정
            m_label->setText((lStr =="0"|m_isFirst)?bStr:lStr+bStr);
            m_isFirst = false;
        }
        break;
    case 3: case 7: case 11: case 15:
        if(m_label != nullptr) m_num1 = m_label->text( );
        m_op = bStr;
        m_isFirst = true;
        break;
    case 13: case 14: break;
    };
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 128
#define ISNUMBER(a)     ( (((a)>='0') && ((a)<='9')) ? 1 : 0 )
#define ISCHARACTER(a)  ( ((((a)>='A') && ((a)<='Z')) || (((a)>='a')&&((a)<='z'))) ? 1 : 0)

typedef unsigned long ULONG;
typedef long          LONG;
typedef char          CHAR;

typedef struct tagTESTCharNode
{
    CHAR cChar;
    struct tagTESTCharNode *next;
}TEST_CHAR_S;

CHAR gacBuf[MAXCHAR] = {0};

int main()
{
    ULONG ulIndex;
    ULONG ulStrLen;
    TEST_CHAR_S *pstCharHander;
    TEST_CHAR_S *pstCharTemp;
    TEST_CHAR_S *pstCharNode;
    
    TEST_CHAR_S *pstNumberHander;
    TEST_CHAR_S *pstCharacterHander;
    TEST_CHAR_S *pstNumberTemp;
    TEST_CHAR_S *pstCharacterTemp;

    ULONG ulNumberFirst = 0;
    ULONG ulCharacterFirst =0;
    
    

    while (scanf("%s", gacBuf) != EOF)
    {
        ulStrLen = strlen(gacBuf);
        
        pstCharHander = (TEST_CHAR_S *)malloc(sizeof(TEST_CHAR_S));
        if(NULL == pstCharHander)
        {
            printf("malloc error in 0\n");
            return 1;
        }
        pstCharHander->next = NULL;
        pstCharTemp = pstCharHander;
        
        for (ulIndex = 0; ulIndex < ulStrLen; ++ulIndex)
        {
            pstCharNode = (TEST_CHAR_S *)malloc(sizeof(TEST_CHAR_S));
            if (NULL == pstCharNode)
            {
                printf("malloc error in %ld\n", ulIndex);
                break;
            }
            pstCharNode->cChar = gacBuf[ulIndex];
            pstCharNode->next = NULL;

            pstCharTemp->next = pstCharNode;
            pstCharTemp = pstCharNode;
        }

/*
        pstCharTemp = pstCharHander->next;
        while(pstCharTemp != NULL)
        {
            printf("%c", pstCharTemp->cChar);
            pstCharTemp = pstCharTemp->next;            
        }
*/
        pstCharTemp = pstCharHander;

        while (NULL != pstCharTemp->next)
        {
            if (1 == ISNUMBER((pstCharTemp->next)->cChar))
            {
                if (0 == ulNumberFirst)
                {
                    pstNumberHander = pstCharTemp->next;
                    pstNumberTemp = pstNumberHander;
                    ulNumberFirst = 1;
                }
                else
                {
                    pstNumberTemp->next = pstCharTemp->next;
                    pstNumberTemp = pstCharTemp->next;
                }

                pstCharTemp = (pstCharTemp->next)->next;
                continue;
            }

            if (1 == ISCHARACTER((pstCharTemp->next)->cChar))
            {
                if (0 == ulCharacterFirst)
                {
                    pstCharacterHander = pstCharTemp->next;
                    pstCharacterTemp = pstCharacterHander;
                    ulCharacterFirst = 1;
                }
                else
                {
                    pstCharacterTemp->next = pstCharTemp->next;
                    pstCharacterTemp = pstCharTemp->next;
                }
                
                pstCharTemp = (pstCharTemp->next)->next;
                continue;
            }
            
            pstCharTemp = pstCharTemp->next;
        }
        
        pstNumberTemp->next = NULL;
        pstCharacterTemp->next = NULL;

        printf("\n");
        /* 输出数字链表 */
        pstNumberTemp = pstNumberHander;
        while(pstNumberTemp != NULL)
        {
            printf("%c", pstNumberTemp->cChar);
            pstNumberTemp = pstNumberTemp->next;
        }
        
        printf("\n");
        /* 输出字母链表 */
        pstCharacterTemp = pstCharacterHander;
        while(pstCharacterTemp != NULL)
        {
            printf("%c", pstCharacterTemp->cChar);
            pstCharacterTemp = pstCharacterTemp->next;
        }

        printf("\n");
        /* 输出其他链表 */
        pstCharTemp = pstCharHander->next;
        while(pstCharTemp != NULL)
        {
            printf("%c", pstCharTemp->cChar);
            pstCharTemp = pstCharTemp->next;
        }


        
        
    }

    return 0;
}











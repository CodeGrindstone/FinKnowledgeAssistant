from fastapi import FastAPI
from pydantic import BaseModel, Field
from typing import List, Optional
import time

"""
运行： uvicorn main:app --host 0.0.0.0 --port 8000
"""


app = FastAPI(title="Mock RAG Service", version="0.1.0")


class AskOptions(BaseModel):
    top_k: int = 5
    temperature: float = 0.2


class AskRequest(BaseModel):
    question: str = Field(..., min_length=1)
    user_id: Optional[str] = "anonymous"
    trace_id: Optional[str] = None
    options: AskOptions = AskOptions()


@app.get("/health")
def health():
    return {
        "code": 0,
        "message": "success",
        "data": {
            "status": "ok",
            "service": "mock_python_rag",
            "version": "0.1.0"
        }
    }


@app.post("/rag/ask")
def rag_ask(req: AskRequest):
    start = time.time()

    # 模拟处理耗时
    time.sleep(0.03)

    answer = (
        f"【Python Mock RAG回答】用户({req.user_id})提问：{req.question}。"
        f"当前使用 top_k={req.options.top_k}, temperature={req.options.temperature}。"
        "后续将替换为 LangChain + 向量检索 + 大模型生成。"
    )

    latency_ms = int((time.time() - start) * 1000)

    return {
        "code": 0,
        "message": "success",
        "data": {
            "answer": answer,
            "model": "mock-python-rag",
            "latency_ms": latency_ms,
            "confidence": 0.88,
            "references": [
                {
                    "title": "金融术语手册（示例）",
                    "source": "mock_kb",
                    "chunk_id": "finance_terms#12",
                    "score": 0.91
                },
                {
                    "title": "证券投资基础（示例）",
                    "source": "mock_kb",
                    "chunk_id": "security_basic#05",
                    "score": 0.87
                }
            ]
        }
    }
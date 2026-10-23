/*!!不要修改该内部内容!!*/

进入工程先读 `PROJECT_CONTEXT.md`、`ARCHITECTURE.md` 和 `Tool.md`。
、`Agents/PROJECT_CONTEXT.md` 和 `Agents/ARCHITECTURE.md`。

- 当前 HAL 工程是唯一代码事实来源。
- 以当前 HAL 工程和 Git diff 为准。
- 任务开始时只读 `NowTask.md`和 `STATE.md` 的当前项；需要追溯时才按需查 `DECISIONS.md`。
- 模型职责按用户要求动态分配；未要求多 Agent 时不自行扩大协作流程。
- 修改前核对当前 diff，修改后记录实际改动和验证层级；不把编译、烧录和物理行为混为一谈。
- 分层硬约束：`Algorithm` 只放算法，`APP` 放业务，`BSP` 放板级配置和外设驱动放设备驱动
- 不把临时测试、打印、参数映射或业务分支留在驱动和 `app_main.c` 中。

/*!!不要修改该内部内容!!*/


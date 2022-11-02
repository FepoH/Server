# git

## git配置
git config 
--list      查看配置信息
--unset     重置配置
--global    用户范围
--alias     命令别名

user.name   配置用户名
user.email  配置邮箱


//配置用户git时忽略的文件
git --config --global core.excludesfile ~/.gitignore_global
//配置当前项目git忽略的文件
创建`.gitignore`项目根目录



## git 命令
### git help 
-a      所有命令
-g      查看使用手册
add     具体命令

### git diff
git diff --staged   //使用暂存区和仓库进行对比,暂存区无东西,则无差异
git diff            //使用暂存区和仓库进行对比
                    //若原本的修改已经添加到暂存区,之后又发生了修改,则对比的是,工作区和暂存区对比


### git checkout
//不仅仅是删除的文件,被修改的文件一样可以用
git checkout HEAD -- filename   //恢复被删除的文件,恢复的最近的一次提交
git checkout HEAD^ -- filename  //恢复被删除的文件,恢复到最近的第二次提交  


### git add, git rm, git mv
添加,删除,移动文件
git rm --cached filename        //删除暂存区文件

### git log
git reflog          //简短的日志
git log --oneline   //一行一行显示日志
git log --oneline -- decorate  --all //更加详细的显示一行行日志


### git revert
+ 恢复到指定版本
+ 执行此命令后,会打开编辑器编辑`commit提交信息`
git revert ID   //ID就是`git log --oneline`中的指定版本的ID

### git reset
--soft      //只改变指针指向的版本
--mixed     //改变指针以及暂存区
--hard      //改变指针以及暂存区以及工作区

### git status
查看当前状态,分支,修改,add,cimmit等

### git branch
查看当前分支,`*`代表当前分支
git branch branch_name      //创建分支
git checkout branch_name    //切换分支
git diff branch1..branch2   //比较两个分支的差异,<font color=red>注意:中间有两个点</font>
                            //后面加`filename`,只比较某个文件
git branch -m branch_name1 branch_name2     //重命名分支
git branch -d branch_name                   //删除分支


### git stash
暂时保存文件         //应用场景,修改的文件还不想提交,暂存一下
git stash --list
git stash show -p   //比较区别
git stash apply stash_id    //恢复工作进度
git stash pop stash_id      //删除暂存文件


+ git不会跟踪空白目录
+ 对文件进行修改之后,要重新添加
+ `HEAD`是表头,`HEAD^`是表头的上一次提交,通体第n次提交


+ 工作区:就是当前项目的文件
+ 暂存区:就是`add`后的区域
+ 仓库:就是已经`commit`的各个版本
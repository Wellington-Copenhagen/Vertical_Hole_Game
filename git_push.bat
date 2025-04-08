rem add commit pushを一度にやりたい
call git add *
call git commit -m %1
rem call git push origin ECS-applied:ECS-applied
pause
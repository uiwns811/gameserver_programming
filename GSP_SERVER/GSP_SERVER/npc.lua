myid = 99999;
level = -1;
hp = -1;

function set_uid(x)
	myid = x;
end

function CreatePF()
	level = 1;
	hp = 50;
end

function CreatePL()
	level = 2;
	hp = 100;
end

function CreateAF()
	level = 2;
	hp = 200;
end

function CreateAL()
	level = 3;
	hp = 300;
end

function Initialize(attacktype, movetype)
	if (attacktype == 0 and movetype == 0) then
		CreatePF();
	elseif (attacktype == 0 and movetype == 1) then
		CreatePL();
	elseif (attacktype == 1 and movetype == 0) then
		CreateAF();
	elseif (attacktype == 1 and movetype == 1) then
		CreateAL();
	end
	return level, hp;
end
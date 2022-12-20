myid = 99999;
level = -1;
hp = -1;

function set_uid(x)
	myid = x;
end

function CreatePF()
	level = 1;
	hp = 10;
end

function CreatePL()
	level = 3;
	hp = 50;
end

function CreateAF()
	level = 2;
	hp = 100;
end

function CreateAL()
	level = 2;
	hp = 150;
end

function Init()
	math.randomseed(os.time());
	type = math.random(0, 3);
	x, y = API_InitializePos();
	if (type == 0) then
		CreatePF();
	elseif (type == 1) then
		CreatePL();
	elseif (type == 2) then
		CreateAF();
	elseif (type == 3) then
		CreateAL();
	end
	return type, x, y, level, hp;
end
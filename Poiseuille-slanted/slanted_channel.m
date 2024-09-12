%This script is used to generate the .vertex file representing the slanted
%channel

th = pi/6; %Degree of rotation associated with the slanted channel

H = 1.0;
L = 1.0;


NFINEST = 32;

dx = (1/NFINEST); %The width of the Cartesian background grid

mfac = 1.0; %Ratio of the Lagrangian mesh spacing to the Cartesian mesh spacing
DX = dx*mfac;
%Compute the length of the slanted channel walls
tot_length = sqrt(1 + tan(th)*tan(th));
N_points = 1 + round(tot_length/DX);
x1 = zeros(N_points,1);
y1 = x1;
x2 = x1;
y2 = x1;
ds = tot_length/(N_points - 1);
s1 = 0:ds:tot_length;
for i = 1:N_points
    x1(i) = s1(i)*cos(th);
    y1(i) = s1(i)*sin(th);
    x2(i) = s1(i)*cos(th);
    y2(i) = s1(i)*sin(th) + sin(th)*tan(th)*H + cos(th)*H;
end
x1 = x1(2:end-1);
x2 = x2(2:end-1);
y1 = y1(2:end-1);
y2 = y2(2:end-1);
N_IB = length(y1) + length(y2);



%See what the channel looks like

plot(x1,y1,'o')
hold on
plot(x2,y2,'o')

vertex_fid = fopen(['Slanted_Channel' num2str(NFINEST) '.vertex'],'w');
fprintf(vertex_fid, '%d\n', N_IB);
for i = 1:length(x1)
     fprintf(vertex_fid,'%1.16e %1.16e\n',x1(i),y1(i));
     fprintf(vertex_fid,'%1.16e %1.16e\n',x2(i),y2(i));
end



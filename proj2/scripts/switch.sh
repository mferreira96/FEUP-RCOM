conf t
vlan 50
end

conf t
vlan 51
end

conf t
interface fastethernet 0/1
switchport mode access
switchport access vlan 50
end

conf t
interface fastethernet 0/2
switchport mode access
switchport access vlan 50
end

conf t
interface fastethernet 0/3
switchport mode access
switchport access vlan 51
end

conf t
interface fastethernet 0/4
switchport mode access
switchport access vlan 51
end

configure terminal
interface fastethernet 0/5
switchport mode access
switchport access vlan 51
end

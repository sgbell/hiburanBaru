$setup_script = <<SCRIPT
yum install epel-release -y
yum groupinstall "Server with GUI" -y
yum groupinstall "Xfce" -y
yum groupinstall "C Development Tools and Libraries" -y
yum -y install mesa-libGL-devel cmake3 lightdm gcc-c++ boost-devel screen git
wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run
systemctl set-default graphical.target
systemctl disable gdm
systemctl enable lightdm
# need to set lightdm to autologin, because I'm lazy
echo "[SeatDefaults]" > /etc/lightdm/lightdm.conf
echo "autologin-user=vagrant" >> /etc/lightdm/lightdm.conf
echo "autologin-user-timeout=0" >> /etc/lightdm/lightdm.conf
echo "user-session=xfce" >> /etc/lightdm/lightdm.conf
systemctl start lightdm
SCRIPT

Vagrant.configure("2") do | config |
  config.vm.box = "centos/7"
  config.disksize.size = '40GB'

  config.vm.define :hiburanBaru do |hiburanBaru_config|
     hiburanBaru_config.vm.provider "virtualbox" do |vb|
       vb.name = "hiburanBaru"
       vb.gui = true
       vb.memory = "1024"
     end
     hiburanBaru_config.vm.hostname = 'hiburanBaru.lan'
     hiburanBaru_config.vm.synced_folder ".", "/srv"
     hiburanBaru_config.vm.provision "shell", inline: $setup_script
  end
end

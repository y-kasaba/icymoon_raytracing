      program trc00a
c
***********************************************************************
*                                                                     *
*            ray tracing program - auto.calc.         ver. 1.1        *
*                                                                     *
*           'trc00a.f.f': CONNERNEY O6 & VIP4 model                   *
*                                             Misa 2000/02/19 (Sat)   *
***********************************************************************
*
*
      implicit real*8 (a-h,o-z)
      real*8   x0(3),k0(3),b0(3),n,ref,n0,noff,rrr,x00(3),b00(3)
      real*8   x(3),k(3),b(3),dbdx(3,3),dndx(3),kox,koy,koz
      real*8   dgdx(3),dgdk(3),dgdw,dt,dx(3),dk(3),dxi(3),dki(3)
      integer  i,j,ns,ipp
      character    mode*1,ojc*1,finame*18,finam2*18
      character*4  cutf,fpla,fuhr,frx,fpu,fpfc
      character*16 nemdl,mdl1,mdl2,mdl3,mdl4,mdl5,mdl6,mdl7,mdl8,mdl9
*
      real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
      common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &              alfa,ox,bunsi,bunbo,root
*
      real*8  pai,rad,pp,pc,c,a,arj0,brj0
      common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
      real*8  pols(2,0:31),polc(2,0:31),
     &        roff,mus,muc,dls,dlc,lns,lnc,tls,tlc
      common /polyco/pols,polc,roff,mus,muc,dls,dlc,lns,lnc,tls,tlc
*
      real*8  e,ep,me
      common /const1/e,ep,me
*
      integer sw1,sw2
      common /switch/sw1,sw2
*
      parameter ( fpla=' fp ',fuhr='fuhr',frx='fr-x',fpu='puhr' )
      parameter ( fpfc='fpfc' )
c
cc redefine @2000/2/21
      parameter ( mdl1='2.E5 H=960km+mag' )
      parameter ( mdl2='2.E6 H=960km+mag' )
      parameter ( mdl3='2.E7 H=960km+mag' )
      parameter ( mdl4='6.E5 H=960km+mag' )
      parameter ( mdl5='2.E7 H=640Km    ' )
      parameter ( mdl6='6.E5 H=640Km    ' )
      parameter ( mdl7='2.E7 H=960Km    ' )
      parameter ( mdl8='6.E5 H=960Km    ' )
      parameter ( mdl9='6.E5 H=725Km    ' )
*
      pai   = 3.141592653589793d0
      rad   = pai/180.d0     !// ラジアンへの変換係数
      c     = 2.99792458d8   !// 光の速さ
      a     = 71372.d3       !// 木星の（平均？）半径
      arj0  = 71398.d3       !// 木星の長径
      brj0  = 66770.d3       !// 木星の短径
      e     = 1.60217759d-19 !// 電気素量
      ep    = 8.85418782d-12 !// 誘電率
      me    = 9.1093922 d-31 !// 電子の質量
*
      bb = 0.d0
      kk = 0.d0
      bk = 0.d0
      co2 = 0.d0
      si2 = 0.d0
      wp2 = 0.d0
      wc2 = 0.d0
      bunsi = 0.d0
      bunbo = 0.d0
      root = 0.d0
*
      ipp = 0
*
* Jovicentric declination of the earth
      de = 0.d0
*
      open(15,file='calist00',status='old')
           read(15,'(a1)') mode
           read(15,*) ratio
           read(15,'(a10)') finame
           read(15,'(a10)') finam2
      close(15)
*
*****周波数の決定：freqは周波数、w = 2πfreqで角周波数。
      if ((finame(2:2).eq.'O').or.(finame(2:2).eq.'o')) freq=20.d6
      if ((finame(2:2).eq.'X').or.(finame(2:2).eq.'x')) freq=20.d6
      if  (finame(2:2).eq.'3') freq=30.d6
      w = 2.d0*pai*freq
*
*switches
*  sw1 : L-O mode cut-off selection /1:plasma /2:uhr /3:p-uhr /4:fp/fc
*  sw2 : Jovian electron dencity model selection
c
* Ne modeles:
*        ionosphere profile : Atreya,Donahue(1976), Cravens, Horanyi
*                   scale height : Melrose(1991)
*        magntosphere       : Sentman,Geortz(1978)
*
*        1 = thin +mag. H=960km Ne_max=2e5
*        2 = moder+mag. H=960km Ne_max=2e6
*        3 = dense+mag. H=960km Ne_max=2e7
*        4 = thin_med+mag. H=960km Ne_max=6e5
*        5 = dense      H=640km Ne_max=2e7
*        6 = thin-med.  H=640km Ne_max=6e5
*        7 = dense      H=960km Ne_max=2e7
*        8 = thin-med.  H=960km Ne_max=6e5
*        9 = thin-med.  H=725km Ne_max=6e5
*
      sw1 = 3
      if (sw1.eq.1) cutf=fpla
      if (sw1.eq.2) cutf=fuhr
      if (sw1.eq.3) cutf=fpu
      if (sw1.eq.4) cutf=fpfc
*
      read(finame(8:8),'(i1)') sw2
      if (sw2.eq.1) nemdl=mdl1
      if (sw2.eq.2) nemdl=mdl2
      if (sw2.eq.3) nemdl=mdl3
      if (sw2.eq.4) nemdl=mdl4
      if (sw2.eq.5) nemdl=mdl5
      if (sw2.eq.6) nemdl=mdl6
      if (sw2.eq.7) nemdl=mdl7
      if (sw2.eq.8) nemdl=mdl8
      if (sw2.eq.9) nemdl=mdl9
*
      if ((finame(7:7).eq.'C').or.(finame(7:7).eq.'c')) ojc='c' !O6
      if ((finame(7:7).eq.'V').or.(finame(7:7).eq.'v')) ojc='v' !VIP4
*
*   * Fourier coefficients for approximation (magnetic latitude)
*
      if (sw2.le.4) call polset(ojc)
*
      pp = e*e/ep/me/w/w
      pc = e*e/me/me/w/w
*
   10 t = 0.d0
*
      write(6,*) '*** Jovian ray trace exec. ne: ',nemdl,' / b: ',ojc
*
         if ((mode.eq.'O').or.(mode.eq.'o')) go to 60
         if ((mode.eq.'X').or.(mode.eq.'x')) go to 70
         if ((mode.eq.'E').or.(mode.eq.'e')) go to 999
*
   60 write(6,*) '                             o-mode selected'
      ox = 1.d0
      go to 90
*
   70 write(6,*) '                             x-mode selected'
      cutf = frx
      ox = -1.d0
      sw1 = 0
*
*     ratio = trace starting freq. multiple ratio
*     the freq. of each mode wave on the starting point is equal to
*     'ratio' times larger than the local cut-off freq. on the point.
*     if sw3 is selected, start point is equal to where local
*     fp*(1-ratio)+fuhr*ratio is equal to f.
*
   90 continue
*
      write(6,*) 'Fcut:',ratio,'*',cutf
      write(6,*) 'Freq:',int(freq/1.d6)
      write(6,*) 'outfile: ',finame,' ',finam2
      finame='../DATA/'//finame
      finam2='../DATA/'//finam2
*
      if (sw1.eq.4) then
         write(6,*) 'fp/fc ratio:',ratio
         freq0 = freq
         goto 461
      endif
*
      freq0 = freq/ratio
      if (sw1.eq.3) freq0 = freq
  461 w0 = freq0*2.0d0*pai
*
      open(1,file=finame,status='unknown')
         rewind(1)
  191    read(1,*,end=1991) da1
         goto 191
 1991    continue
         backspace(1)
      open(2,file=finam2,status='unknown')
         rewind(2)
  192    read(2,*,end=1992) da2
         goto 192
 1992    continue
         backspace(2)
*
         write(1,51) freq,ox,ratio,de,sw1,sw2
         write(2,51) freq,ox,ratio,de,sw1,sw2
   51    format(f10.0,f5.0,f7.3,f5.0,2i2)
*
      close(1)
      close(2)
*
*   ***  calc. loop ***/*ここからメインループ*/
*
*     /*南北の選択*/
      do 220 ins= 1,2
         ns = ins
         iok = 0
         ixxx= -180
*
*        /*発生させる磁力線を変えるループ*/
         do 221 isub=ixxx,540,30
*
            isin = 0
*
            open(1,file=finame,status='unknown')
                    rewind(1)
   91               read(1,*,end=991) da1
                    goto 91
  991               continue
                    backspace(1)
            open(2,file=finam2,status='unknown')
                    rewind(2)
   92               read(2,*,end=992) da2
                    goto 92
  992               continue
                    backspace(2)
*
c           if ((isub.ge.-180).and.(isub.le.-90)) iwt=2
c           if ((isub.ge.- 60).and.(isub.le. 90)) iwt=1
c           if ((isub.ge. 120).and.(isub.le.270)) iwt=2
c           if ((isub.ge. 300).and.(isub.le.450)) iwt=1
c           if ((isub.ge. 480).and.(isub.le.540)) iwt=2
*
            write(6,*) 's/n ',ins,' sub-io ',isub,' ok no.',iok,finame
*
            iqqq=0
*
*           /*観測者のいる経度を変えるループ*/
            do 222 icml=iqqq,360,10
*
               iio=icml+(180-isub)
               if ((iio.lt.0).or.(iio.gt.360)) go to 222
               cml65 = dble(icml)
               aio = dble(iio)
*
               fai65 = aio + 180.d0 - cml65
*
               ix = 0
               iy = 0
               aa0 = a
*
               md = 0
               ish = 0
*
               if (isin.eq.0) then
                     call jtrc(iwt,freq0,cml65,aio,de,ns,x0,b0,tf,theta
     &                    ,ojc,ish,ox,ix,iy,noff,aa0,ggg,md,ratio,frat)
                     noff = ggg
                     aai0 = aa0
                     cml0 = cml65
                     isin = 1
                     do ki=1,3
                        x00(ki) = x0(ki)
                        b00(ki) = b0(ki)
                     enddo
                  else
                     do ki=1,3
                        x0(ki) = x00(ki)
                        b0(ki) = b00(ki)
                     enddo
                     dcml = cml65-cml0
                     call jfc(x0,dcml)
                     thth = datan2(x0(3),x0(2))
                     ryz  = dsqrt(x0(2)*x0(2)+x0(3)*x0(3))
                     par0 = arj0*arj0*dsin(thth)*dsin(thth)
     &                     +brj0*brj0*dcos(thth)*dcos(thth)
                     rell = arj0*brj0/dsqrt(par0)+276.d3
                     if ((x0(1).le.0.d0).and.(ryz.le.rell)) ish = 1
                     call jfc(b0,dcml)
               endif
*
               aat0 = aai0
               aa0  = aai0
*
               if (ish.ne.0) then
*                  write(6,802)ns,isub,icml,iio
* 802              format(i2,3i4,' under surface ' )
                   goto 222
               endif
*
               bb = tf*tf
*
               call jec(x0,-de)
               call jfc(x0,-cml65)
               call axcs(x0,r,theta,fai65)
*
               call dens(x0,theta,n0,ns,ifl,rat,ix,iy,
     &                   noff,aa0,ggg,md,ojc)
*
               if (ifl.eq.1) then
*                  write(6,811) ns,isub,icml,iio
* 811              format(i2,3i4,' under surface limit ')
                   goto 222
               endif
*
               call cutoff(tf,n0,ox,e,ep,me,fcut,ratio)
*
               if ( freq.le.fcut ) then
                   write(6,*) freq,fcut
                   write(6,803)ns,isub,icml,iio
  803              format(i2,3i4,' under fcut ')
                   goto 222
               endif
*
               md = 1
*
*　　　　　　　/*磁力線に対する角度を振る*/
               do 223 ialfa = 60,120,10
*
                  alfa = dble(ialfa)
*
*　　　　　　　　/*磁力線周りに角度を振る*/
                  do 224 ib = 1,7
*
                     aa0 = aat0
                     ix = ib
                     iii = ib/2
                     ii0 = mod(ib,2)
                     ii1 = ii0*2-1
                     ibeta = 5*iii*ii1
                     beta = dble(ibeta)
*
                     if (ib.eq.1) then
                           bb = tf*tf
                           iout = 0
                           call kinit(b0,n0,k0,beta,iout)
                           if (iout.eq.1) then
                              write(2,657) ns,isub,icml,iio,ialfa,ibeta
  657                         format (i2,5i4,' non K')
                              goto 223
                           endif
                           if (iout.eq.2) then
                              write(2,657) ns,isub,
     &                                     icml,iio,ialfa,ibeta
                              goto 223
  658                      format (i2,5i4,' no proper K, not continue')
                           endif
                           kox = k0(1)
                           koy = k0(2)
                           koz = k0(3)
                        else
                           call axcs(b0,rb,thetab,faib)
                           k0(1) = kox
                           k0(2) = koy
                           k0(3) = koz
                           call krot(k0,thetab,faib,beta)
                     endif
*
                     n = n0
                     do i=1,3
                        x(i) = x0(i)
                        k(i) = k0(i)
                     enddo
*
                     call jec(k,-de)
                     call jfc(k,-cml65)
*
                     dt0=1.d-6
                     dtl=1.d-2
                     dt =dt0
*
*                     rlim=.000375d0
                     rlim=.005d0 !960117 confirmed with trc96plp.f
                     rlma=1.d0+rlim
                     rlmi=1.d0-rlim
                     ruma=1.d0+rlim/2.5d0
                     rumi=1.d0-rlim/2.5d0
*
                     ifla=0
*
*　　　　　　　　　　/*実際に微分方程式を解く*/
*// 変数の意味
*//	ic     : カウンタ？
*//	theta  : 波数ベクトルが磁場と成す角度
*//	rad    : degreeからラジアンへの変換係数
*//	arj0   : 木星半径（長径側）
*//	brj0   : 木星半径（短径側）
*//	x(1-3) : 波動の現在位置をしめす座標配列
*//	dis    : 星中心からの、x( x(1),x(2),x(3) )までの距離。
*//	sur    : 星表面の、星中心からの距離
*//	ipp    : ？

                     do 100 ic = 1,20000
                        iy = ic
*
                        if (ojc.eq.'c') call magfo6(x,b,dbdx,theta) !// Connerney O6 model
                        if (ojc.eq.'v') call magfv4(x,b,dbdx,theta) !// Connerney VIP4 model (dB/dr)
*
                        !// 波動位置の、木星表面からの距離を計算。
                        !// arjとbrjは、木星を楕円と考えた時の長径と短径で、
                        !// そこから現実の木星表面までの距離を導き出している。
                        xxx=dsin(theta*rad)*dsin(theta*rad)/arj0/arj0
                        zzz=dcos(theta*rad)*dcos(theta*rad)/brj0/brj0
                        sur = 1.d0/dsqrt( xxx+zzz )
                        dis = dsqrt(x(1)*x(1)+x(2)*x(2)+x(3)*x(3))
                        alt = dis - sur
*
                        if (alt.le.276.01d3) then !// 近すぎると星に落ちる？
                           id1 = ialfa
                           id2 = ibeta
                           write(6,709)ns,isub,icml,iio,id1,id2
  709                      format(i2,5i5,' ray into jupiter')
                           goto 224               !// 224に移動で強制的に次の磁角へ。
                        endif
*
                        call dens(x,theta,n,ns,ifl,rat,
     &                            ix,iy,noff,aa0,ggg,md,ojc)
     					!// getDensity()? 多分、グローバル変数 n に密度を格納する。

                        call dodndx(x,dndx,ns,ix,iy,
     &				          noff,aa0,ggg,md,ojc)                   !// do "dn/dx" ？結果はdndxに格納
                        call bkang(b,k)
                        call refra(n,ref,ipp)                        !// なにかしこたま初期化
c                       write(6,*) iy,'ref',real(ref)
*
                        if (ipp.eq.1) then
                            write(6,*) 'evanecent occur ! ',iy        !// evanescent の誤植？
                            write(2,656) ns,isub,icml,iio,ialfa,ibeta !// だとすると"消失"か
  656                       format(i2,5i4,' evnc')
                            ipp = 0
                            goto 224
                        endif
*
                        call dodgdw(dgdw)               !//dG/dw == dG/df ?
                        call dodgdx(b,k,dndx,dbdx,dgdx) !//dG/dx == dG/dr ? 結果はdgdxに格納
                        call dodgdk(b,k,dgdk)           !//dG/dk
*
                        do j=1,3
                           dx(j) = - dgdk(j)/dgdw * dt
                           dk(j) =   dgdx(j)/dgdw * dt
                        enddo
*
                        dvn=dsqrt(dx(1)*dx(1)+dx(2)*dx(2)+dx(3)*dx(3))
     &                      /dt
                        dkkn=dsqrt(dk(1)*dk(1)+dk(2)*dk(2)+dk(3)*dk(3)) !使ってない変数
     &                      /dt
*
                        if (ic.eq.1) then
                           do j=1,3
                              dxi(j)=dx(j)
                              dki(j)=dk(j)
                           enddo
                           go to 28
                        endif
*
                        ratv= dvn/dvr   !１回前のステップとの比
                        ratk= dkkn/dkkr
*
***                     時間分解能の調整
                        if ((ratv.gt.rlma).or.(ratv.lt.rlmi)) then
                              dt=dt/2.d0
                              dvn=dvr
                              dkkn=dkkr
                              do j=1,3
                                 x(j) = x(j)-dxi(j)
                                 k(j) = k(j)-dki(j)
                                 dxi(j)=dxi(j)/2.d0
                                 dki(j)=dki(j)/2.d0
                              enddo
                           else
                              do j=1,3
                                 dxi(j)=dx(j)
                                 dki(j)=dk(j)
                              enddo
                              if ((ratv.lt.ruma).and.(ratv.gt.rumi).and.
     &                            (dt.le.dtl*.65d0)) then
                                  dt=dt*1.5d0
                                  do j=1,3
                                     dxi(j)=dx(j)*1.5d0
                                     dki(j)=dk(j)*1.5d0
                                  enddo
                              endif
                        endif
*
   28                   dvr=dvn
                        dkkr=dkkn
*
                        do j=1,3
                           x(j)=x(j)+dxi(j)
                           k(j)=k(j)+dki(j)
                        enddo
*
                        rrr=dsqrt((x(1)/arj0)**2+(x(2)/arj0)**2
     &                           +(x(3)/brj0)**2)
                        if (rrr.gt.2.5d0) go to 101
*
  100                continue
*
                     write(6,804)ns,isub,icml,iio,ialfa,ibeta
                     write(2,804)ns,isub,icml,iio,ialfa,ibeta
  804                format(i2,5i4,' over')
                     go to 224
*
  101                continue
c
                     call jfc(dx,cml65)
                     call jec(dx,de)
*
                     drr = dsqrt(dx(1)*dx(1)+dx(2)*dx(2)+dx(3)*dx(3))
                     faix = datan2(dx(2),dx(1))/rad
                     coth = 90.d0-dacos(dx(3)/drr)/rad
                     afaix = dabs(faix)
                     acoth = dabs(coth)
*
                     if ((faix.gt.7.5d0).or.(faix.lt.-7.5d0)) go to 229
                     if ((coth.gt.7.5d0).or.(coth.lt.-7.5d0)) go to 229
                     write(1,701)ns,isub,icml,iio,ialfa,ibeta,faix,coth
*                    write(6,701)ns,isub,icml,iio,ialfa,ibeta,faix,coth
  701                format(i2,5i4,2f6.1)
                     iok=iok+1
*
  229                continue
*
  224             continue
*
  223          continue
*
               write(6,603)ns,isub,icml,iio,iy,ialfa,faix,coth,finame
  603          format(' ns',i2,' sub',i4,' cml',i4,' io',i4,' rot',
     &                i5,' alf',i4,' fai',f6.1,' cot',f6.1,a9)
*
  222       continue
*
            close(1)
            close(2)
*
  221    continue
*
  220 continue
*
            open(1,file=finame,status='unknown')
                    rewind(1)
   93               read(1,*,end=993) da1
                    goto 93
  993               continue
                    backspace(1)
            open(2,file=finam2,status='unknown')
                    rewind(2)
   94               read(2,*,end=994) da2
                    goto 94
  994               continue
                    backspace(2)
*
  999 write(6,*) 'calc normal end'
      write(1,*) 'calc normal end'
      write(2,*) 'calc normal end'
c
      close(1)
      close(2)
*
      stop
      end
*
*
*
***********************************************************************
*                                                                     *
      subroutine jtrc(iwt,freqs,cml65,aio1,de1,ns,fx,fb,tf,theta,
     &                ojc,ish,ox,ix,iy,noff,aa0,ggg,md,rt,frat)
*                                                                     *
*              input : freqs (mhz)                                    *
*                      cml65 cml(1965) (deg)                          *
*                      aio1  Io-phase  (deg)                          *
*                      de1   Jovicentric decli. of earth (deg)        *
*                      ns    north->2 or south->1                     *
*                      ox    L-O:'o',R-X:'x'                          *
*                      ix,iy loop counter                             *
*                      rt    ratio; fcut/rt(L-O),fcut*rt(R-X)         *
*                      md    =0 (in prog. jtrc)                       *
*                      noff  =0 (in prog. jtrc)                       *
*                                                                     *
*              output: fx(3) pos. of foot of ift (x axis=earth dir.)  *
*                      theta pos. of foot of ift (spherical co-ord.)  *
*                      fb(3) mag. of foot of ift (x axis=earth dir.)  *
*                      br, btheta, bfai (spherical axis)              *
*                      tf    magnetic total force (tesla)             *
*                      frat  ratio; dividing ratio of fp---fuhr       *
*                      aa0   distance from center of jup. to surface  *
*                      ggg   defference of foot point latitude        *
*                                       between calc. & real value    *
*                                                                     *
***********************************************************************
*
c        **************************************
c        *        IFT map   1983-10-17        *
c        *                    (ver.no.1.1)    *
c        *       ( PC-9801 version no.1.1 )   *
c        *                 arranged by misa   *
c        *                     1990/1/10      *
c        **************************************
c
         implicit real*8(a-h,o-z)
         real*8 freq,cml65,fai65,aio1,de1,fx(3),fb(3)
         real*8 psai,wx(3),n,ox,noff,aa0,ggg,rt
         integer ns
         character vis*9,unvis*9,under*9,ishc*9,ojc*1
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         real*8 e,ep,me
         common /const1/e,ep,me
*
         parameter ( vis=' visible ',unvis='unvisible' )
         parameter ( under='under Jup' )
*
         integer sw1,sw2
         common /switch/sw1,sw2
*
         freq = freqs*1.d-6
         aa = (arj0/a)**2
         bb = (brj0/a)**2
c
         de = de1
         aio= aio1
         subio = cml65+180.0d0-aio
c
         ei=(arj0*arj0)/(brj0*brj0)
         thex=datan(ei*dtan(de*rad))
         ctx=dcos(thex)
         stx=dsin(thex)
         ctxde=dcos(thex-de*rad)
         brjde=ctxde/dsqrt((stx*stx)/(arj0*arj0)+(ctx*ctx)/(brj0*brj0))
c
c        *     loop     *
c
         dl = 250.0d3
c
         i=ns
*   * Alfven lead angle
            if (i.eq.1) aio=aio+(5.5d0+3.2d0*dsin((subio-112.d0)*rad))
            if (i.eq.2) aio=aio+(5.7d0-3.6d0*dsin((subio-112.d0)*rad))
c           if ((i.eq.1).and.(iwt.eq.2))
c    &         aio=aio+(5.5d0+3.2d0*dsin((subio-112.d0)*rad))
c           if ((i.eq.2).and.(iwt.eq.1))
c    &         aio=aio+(5.7d0-3.6d0*dsin((subio-112.d0)*rad))
c           if ((i.eq.1).and.(iwt.eq.1))
c    &         aio=aio+(16.9d0-4.d0*dsin((subio-112.d0)*rad))
c           if ((i.eq.2).and.(iwt.eq.2))
c    &         aio=aio+(16.7d0+2.8d0*dsin((subio-112.d0)*rad))
*
         psai=aio+180.0d0
         theta=90.0d0
         r=5.91d0*a
         fai65=psai-cml65
c
         if (ojc.eq.'c') call jfdo6(r,theta,fai65,br,btheta,bfai,tf)
         if (ojc.eq.'v') call jfdv4(r,theta,fai65,br,btheta,bfai,tf)
*
         call axsc(r,theta,fai65,wx)
         call dens(wx,theta,n,ns,ifl,rat,ix,iy,noff,aa0,ggg,md,ojc)
         fp=dsqrt(e*e*n/ep/me)/2.d0/pai
         fc=e*tf/me/2.d0/pai
         if (ox.eq.1.d0) then
               go to (10,11,12,10),sw1
   10          fcut = fp*1.d-6
               go to 13
   11          fcut=dsqrt(fp*fp+fc*fc)*1.d-6
               go to 13
   12          fcut=(fp*(1.d0-rt)+dsqrt(fp*fp+fc*fc)*rt)*1.d-6
            else
               fcut=(fc/2.d0+dsqrt(fp*fp+fc*fc/4.d0))*1.d-6
         endif
   13    freqh = fcut
c
         if (sw1.eq.4) then
             fpc=fp/fc
             if (fpc-rt) 60,60,20
         endif
c
         if (freqh-freq) 60,60,20
c
   60    sign=dble(3-2*i)
c
   40    dtheta=sign*dl*btheta/tf
         dfai  =sign*dl*bfai  /tf
         dr    =sign*dl*br    /tf
c
         cf=dcos(fai65*rad)
         sf=dsin(fai65*rad)
         ct=dcos(theta*rad)
         st=dsin(theta*rad)
c
         drx=cf*ct*dtheta-sf*dfai+cf*st*dr
         dry=sf*ct*dtheta+cf*dfai+sf*st*dr
         drz=  -st*dtheta        +   ct*dr
c
         x=r*st*cf+drx
         y=r*st*sf+dry
         z=r*ct   +drz
c
         r=dsqrt(x*x+y*y+z*z)
         fai65=datan2(y,x)/rad
         theta=dacos(z/r)/rad
c
         if (ojc.eq.'c') call jfdo6(r,theta,fai65,br,btheta,bfai,tf)
         if (ojc.eq.'v') call jfdv4(r,theta,fai65,br,btheta,bfai,tf)
*
         call axsc(r,theta,fai65,wx)
         call dens(wx,theta,n,ns,ifl,rat,ix,iy,noff,aa0,ggg,md,ojc)
*
         if ((rat.le.1.1d0).and.(ox.eq.1.d0)) dl=3.1d3*(rat**46)
*
         if (ifl.eq.1) then
             write(6,*) ' under surface limit'
             ish = 1
             goto 999
         endif
*
         fp=dsqrt(e*e*n/ep/me)/2.d0/pai
         fc=e*tf/me/2.d0/pai
         if (ox.eq.1.d0) then
               go to (14,15,23,14),sw1
   14          fcut = fp*1.d-6
               go to 16
   15          fcut=dsqrt(fp*fp+fc*fc)*1.d-6
               go to 16
   23          fcut=(fp*(1.d0-rt)+dsqrt(fp*fp+fc*fc)*rt)*1.d-6
            else
               fcut=(fc/2.d0+dsqrt(fp*fp+fc*fc/4.d0))*1.d-6
         endif
   16    freqh1 = fcut
c
         if (sw1.eq.4) then
            fpc=fp/fc
            if (fpc-rt) 52,70,32
   52       fpcr = fpc
            go to 40
         endif
c
         if (freqh1-freq) 50,70,30
   50    freqh=freqh1
         go to 40
c
   30    fact=(freqh1-freq)/(freqh1-freqh)
         goto 34
   32    fact=(fpc-rt)/(fpc-fpcr)
   34    x=x-fact*drx
         y=y-fact*dry
         z=z-fact*drz
         r=dsqrt(x*x+y*y+z*z)
         fai65=datan2(y,x)/rad
         theta=dacos(z/r)/rad
   70    continue
c
         if (ojc.eq.'c') call jfdo6(r,theta,fai65,br,btheta,bfai,tf)
         if (ojc.eq.'v') call jfdv4(r,theta,fai65,br,btheta,bfai,tf)
*
         call axsc(r,theta,fai65,wx)
         call dens(wx,theta,n,ns,ifl,rat,ix,iy,noff,aa0,ggg,md,ojc)
*
         fp=dsqrt(e*e*n/ep/me)/2.d0/pai
         fc=e*tf/me/2.d0/pai
         if (ox.eq.1.d0) then
               go to (17,18,21,25),sw1
   17          fcut = fp*1.d-6
               go to 19
   18          fcut=dsqrt(fp*fp+fc*fc)*1.d-6
               go to 19
   21          fcut=(fp*(1.d0-rt)+dsqrt(fp*fp+fc*fc)*rt)*1.d-6
               go to 19
   25          frat=(freqs-fp)/(dsqrt(fp*fp+fc*fc)-fp)
               rt = fp/fc
            else
               fcut=(fc/2.d0+dsqrt(fp*fp+fc*fc/4.d0))*1.d-6
         endif
   19    freqh = fcut
         if (sw1.ne.4) frat=rt
*
         if (sw1.eq.4) write(6,*) 'last fp/fc:',rt,'  p-uhr rat:',frat
*
         psai=fai65+cml65
         if (psai.gt. 180.0d0) psai=psai-360.0d0
         if (psai.le.-180.0d0) psai=psai+360.0d0
c
         call axbsc(fai65,theta,br,btheta,bfai,fb)
         call jfc(fb,cml65)
*
         call jec(fb,de)
*
         cp  = dcos(psai*rad)
         sp  = dsin(psai*rad)
         ct  = dcos(theta*rad)
         st  = dsin(theta*rad)
         cde = dcos(de*rad)
         sde = dsin(de*rad)
c
         ur=br/tf
         utheta=btheta/tf
         ufai=bfai/tf
c
         ctbe=(ct*cp*utheta-sp*ufai+st*cp*ur)*cde
     &       +(ct*ur-st*utheta)*sde
         thbe=dacos(ctbe)/rad
c
         xe=r*( cde*st*cp+sde*ct)
         ye=r*      st*sp
         ze=r*(-sde*st*cp+cde*ct)
c
         fx(1) = xe
         fx(2) = ye
         fx(3) = ze
c
         call shadow(xe,ye,ze,brjde,ish)
*
         if (ish.eq.0) then
               ishc=vis
            else
               ishc=unvis
         endif
*
         rr = (xe/a)**2/aa+(ye/a)**2/aa+(ze/a)**2/bb
         if ( rr.lt.1.d0 ) ishc=under
*
   20    continue
*
  999 return
      end
*
***********************************************************************
*
* Connerney O6 model
*
      subroutine jfdo6(r,theta,fai65,br,btheta,bfai,tf)
*
***********************************************************************
*
c        **************************************************
c        *     Jovian magnetic field -- Connerney O6 model*
c        *                             (Connerney, 1993)  *
c        *       input;                                   *
c        *          r       distance (km)                 *
c        *          theta   colatitude(deg)               *
c        *          fai65   systemIII east longitude(deg) *
c        *       output;                                  *
c        *          (btheta,bfai,br)   mag. field comp.   *
c        *           tf     total force (gauss)           *
c        **************************************************
c
*
         implicit real*8(a-h,o-z)
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         g10= 4.24202D0
         g11=-0.65929D0
c
         g20=-0.02181D0
         g21=-0.71106D0
         g22= 0.48714D0
c
         g30= 0.07565D0
         g31=-0.15493D0
         g32= 0.19775D0
         g33=-0.17958D0
c
         h11= 0.24116D0
c
         h21=-0.40304D0
         h22= 0.07179D0
c
         h31=-0.38824D0
         h32= 0.34243D0
         h33=-0.22439D0
c
         ct=dcos(theta*rad)
         st=dsin(theta*rad)
c
         cf1=dcos(fai65*rad)
         sf1=dsin(fai65*rad)
         cf2=dcos(2.d0*fai65*rad)
         sf2=dsin(2.d0*fai65*rad)
         cf3=dcos(3.d0*fai65*rad)
         sf3=dsin(3.d0*fai65*rad)
c
         ar1=(a/r)**3
         ar2=(a/r)**4
         ar3=(a/r)**5
c
         bthet1=-ar1*(-g10*st
     &              +( g11*cf1+h11*sf1)*ct)
         bfai1 =-ar1*(-g11*sf1+h11*cf1)
         br1   =2.d0*ar1*(g10*ct
     &                 +(g11*cf1+h11*sf1)*st)
c
         p20 =(3.d0*ct*ct-1.d0)/2.d0
         dp20=-3.d0*ct*st
         p21 =1.732050807d0*ct
         dp21=1.732050807d0*(2.0d0*ct*ct-1.0d0)
         p22 =0.866025403d0*st
         dp22=1.732050807d0*ct*st
c
         gh21=g21*cf1+h21*sf1
         gh22=g22*cf2+h22*sf2
c
         bthet2=-ar2*(g20*dp20+gh21*dp21+gh22*dp22)
         bfai2 =-ar2*((-g21*sf1+h21*cf1)*p21
     &         +2.0d0*(-g22*sf2+h22*cf2)*p22)
         br2= 3.0d0*ar2*(g20*p20+gh21*p21*st+gh22*p22*st)
c
         p30 =ct*(5.d0*ct*ct-3.d0)/2.d0
         dp30=3.d0*st*(1.d0-5.d0*ct*ct)/2.d0
         p31 =0.612372435d0*(-5.d0*st*st+4.d0)
         dp31=0.612372435d0*ct*(-15.d0*st*st+4.d0)
         p32 =1.936491673d0*ct*st
         dp32=1.936491673d0*st*(3.d0*ct*ct-1.d0)
         p33 =0.790569415d0*st*st
         dp33=2.371708245d0*st*st*ct
c
         gh31=g31*cf1+h31*sf1
         gh32=g32*cf2+h32*sf2
         gh33=g33*cf3+h33*sf3
c
         bthet3=-ar3*(g30*dp30+gh31*dp31+gh32*dp32+gh33*dp33)
         bfai3=-ar3*((-g31*sf1+h31*cf1)*p31
     &        +2.0d0*(-g32*sf2+h32*cf2)*p32
     &        +3.0d0*(-g33*sf3+h33*cf3)*p33)
         br3=4.0*ar3*(g30*p30+gh31*p31*st+gh32*p32*st+gh33*p33*st)
c
c
         br     = br1+br2+br3
         btheta = bthet1+bthet2+bthet3
         bfai   = bfai1+bfai2+bfai3
*
*        unit convert : gauss->tesla
*
         br     = br*1.d-4
         btheta = btheta*1.d-4
         bfai   = bfai*1.d-4
c
         tf2=br*br+btheta*btheta+bfai*bfai
         tf = dsqrt(tf2)
c
      return
      end
*
***********************************************************************
*
      subroutine axbsc(fai,theta,br,btheta,bfai,b)
*
***********************************************************************
*
         real*8 br,btheta,bfai,fai,theta,ct,st,cf,sf
         real*8 b(3)
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         ct = dcos(theta*rad)
         st = dsin(theta*rad)
         cf = dcos(fai*rad)
         sf = dsin(fai*rad)
*
         b(1) = (st*cf*br+ct*cf*btheta-sf*bfai)
         b(2) = (st*sf*br+ct*sf*btheta+cf*bfai)
         b(3) = (ct   *br-st   *btheta        )
*
      return
      end
*
***********************************************************************
*
      subroutine shadow(xe,ye,ze,brjde,ish)
*
***********************************************************************
*
         real*8  g,xe,ye,ze,brjde
         integer ish
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         g= (ze*ze)/(brjde*brjde)  +(ye*ye)/(arj0*arj0)
c
         ish=0
         if((xe.lt.0.d0).and.(g.lt.1.d0)) ish=1
*
      return
      end
*
***********************************************************************
*
      subroutine kinit(b,n,k,beta,iout)
*
***********************************************************************
*
         integer ipp,iout
         real*8 b(3),k(3),n,ref,beta
         real*8 rb,thetab,faib
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         call refra(n,ref,ipp)
*
         call defk(b,ref,k,iout)
*
         call axcs(b,rb,thetab,faib)
*
         call krot(k,thetab,faib,beta)
*
      return
      end
*
***********************************************************************
*
      subroutine defk(b,ref,k,iout)
*
***********************************************************************
*
*   *******************************************
*   * define initial wave vector k (kx,ky,kz) *
*   *******************************************
*
         integer iout
         real*8 k(3),b(3),p1,p2,p3,p4,babs,ref
         real*8 qqq,k0,sa,co,kx1,kx2
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         k0 = w/c
*
         babs  = dsqrt(bb)
*         write (6,*) 'babs ',babs
*
         co = dcos(alfa*rad) !// alfaは磁力線に対する角度。
*
         p1 = (b(2)*b(2)+b(3)*b(3))*bb
         p2 = -2.d0*(b(2)*b(2)+b(3)*b(3))*b(3)*babs*co*k0*ref
         p3 = k0*k0*ref*ref*b(3)*b(3)*(bb*co2-b(1)*b(1))
*
         qqq = p2*p2-4.d0*p1*p3
         if (qqq.lt.0.d0) qqq=0.d0
*         write (6,*) ' qqq ',qqq
*
         k(3) = (-p2+dsqrt(qqq))/2.d0/p1
         k(2) = b(2)/b(3)*k(3)
         k(1) = dsqrt(k0*k0*ref*ref-k(2)*k(2)-k(3)*k(3))
         p4 = (b(1)*k(1)+b(2)*k(2)+b(3)*k(3))/babs/k0/ref
         sa = dabs(co-p4)
         if ( ( sa.lt.1.d-12 ).and.( k(1).gt.0.d0 ) ) goto 5
*
         k(3) = (-p2-dsqrt(qqq))/2.d0/p1
         k(2) = b(2)/b(3)*k(3)
         k(1) = dsqrt(k0*k0*ref*ref-k(2)*k(2)-k(3)*k(3))
         p4 = (b(1)*k(1)+b(2)*k(2)+b(3)*k(3))/babs/k0/ref
         sa = dabs(co-p4)
         if ( ( sa.lt.1.d-12 ).and.( k(1).gt.0.d0 ) ) goto 5
*
         k(3) = (-p2+dsqrt(qqq))/2.d0/p1
         k(2) = b(2)/b(3)*k(3)
         k(1) = -dsqrt(k0*k0*ref*ref-k(2)*k(2)-k(3)*k(3))
         p4 = (b(1)*k(1)+b(2)*k(2)+b(3)*k(3))/babs/k0/ref
         sa = dabs(co-p4)
         if ( sa.lt.1.d-12 ) then
            iout=1
            goto 5
         endif
*
         k(3) = (-p2-dsqrt(qqq))/2.d0/p1
         k(2) = b(2)/b(3)*k(3)
         k(1) = -dsqrt(k0*k0*ref*ref-k(2)*k(2)-k(3)*k(3))
         p4 = (b(1)*k(1)+b(2)*k(2)+b(3)*k(3))/babs/k0/ref
         sa = dabs(co-p4)
         if ( sa.lt.1.d-12 ) then
            iout=1
            goto 5
         endif
*
   17    write(6,*) 'No k-vector is defined ! ',real(k(1))
         iout = 2
*
    5    continue
*
      return
      end
*
***********************************************************************
*
      subroutine refra(n,ref,ipp)
*
***********************************************************************
*
         integer ipp
         real*8 n,si4,ref
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         wp2 = pp * n
         wc2 = pc * bb
*
		!ここでのalfaは、磁場に対する角度[deg]かな？
		!alfaは、bkang()を呼び出して更新するっぽ。
         co2 = dcos(alfa*rad)*dcos(alfa*rad)
         si2 = 1.d0-co2
         si4 = si2*si2
*
         bunsi = 2.d0*wp2*(1.d0-wp2)
         root = dsqrt( wc2*wc2*si4+4.d0*wc2*co2*(1.d0-wp2)*(1.d0-wp2) )
         bunbo = 2.d0*(1.d0-wp2)-wc2*si2+ox*root
*
         if (bunsi/bunbo.ge.1.d0) then
            ipp = 1
            go to 5
         endif
*
         ref = dsqrt(1.d0-bunsi/bunbo)
*
    5 return
      end
*
***********************************************************************
*
      subroutine krot(k,thetab,faib,beta)
*
***********************************************************************
*
         real*8 k(3),thetab,faib,beta,cost,sint,cosf,sinf
         real*8 kox,koy,koz,kbx,kby,kbz
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         cost = dcos(thetab*rad)
         sint = dsin(thetab*rad)
         cosf = dcos(faib*rad)
         sinf = dsin(faib*rad)
*
         kox = cost*cosf*k(1) + cost*sinf*k(2) - sint*k(3)
         koy = -sinf    *k(1) + cosf     *k(2) + 0.d0
         koz = sint*cosf*k(1) + sint*sinf*k(2) + cost*k(3)
*
         kbx = dcos(beta*rad)*kox - dsin(beta*rad)*koy + 0.d0
         kby = dsin(beta*rad)*kox + dcos(beta*rad)*koy + 0.d0
         kbz = 0.d0               + 0.d0               + koz
*
         k(1) = cosf*cost*kbx - sinf*kby + cosf*sint*kbz
         k(2) = sinf*cost*kbx + cosf*kby + sinf*sint*kbz
         k(3) = -sint    *kbx + 0.d0     + cost     *kbz
*
      return
      end
*
***********************************************************************
*
      subroutine dens(x,theta,n,ns,ifl,r,ix,iy,noff,aa0,ggg,md,ojc)
*
***********************************************************************
*
         implicit real*8(a-h,o-z)
         real*8 x(3),n,theta,dis,sur,xxx,zzz,ggg,ggx
         real*8 nl1,nl2,nu1,nu2,ne1,ne2,noff,aa0
         real*8 alt,r,flat,ramm,rams,g,aaa
         real*8 p1,p2,p3,p4,p5,p6,p7,p8
         character ojc*1
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         integer sw1,sw2
         common /switch/sw1,sw2
*
         !// arjとbrjは、木星を楕円と考えた時の長径と短径で、
         !// そこから現実の木星表面までの距離を導き出している。
         xxx = dsin(theta*rad)*dsin(theta*rad)/arj0/arj0
         zzz = dcos(theta*rad)*dcos(theta*rad)/brj0/brj0
         sur = 1.d0/dsqrt( xxx+zzz )
*
         ifl = 0 !//エラーを示す数値？
*
         dis = dsqrt(x(1)*x(1)+x(2)*x(2)+x(3)*x(3))
         alt = dis - sur !// 波動の、木星表面からの距離＝高さを導出
*
         if (alt.le.276.01d3) then !//主星に近すぎたらだめよ。
            ifl = 1
            goto 999
         endif
*
         r = dis/sur !//単位は[Rj]での距離。
*
         if (sw2.ge.5) goto 5
c
         call maglat(x,ramm,flat,ns,ix,iy,aa0,ojc)
*
         ggg = ramm-flat
*
         rams = 60.d0/(r-1.d0)
         g = dexp(-ramm*ramm/rams/rams)
*
    5    go to (10,20,30,40,50,60,70,80,90),sw2
*
*         * normal thin
c  10       aaa = -(alt-1417.d3)/386.3d3
   10       aaa = -(alt-1417.d3)/960.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 11
            endif
            p1 = dexp(aaa)
   11       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 2.d5*p1/p2
            nu1 = 1.d4*dexp(-7.7d0*(1.d0-1.d0/r))*
     &                     (1.d0-.99d0*dexp(-alt/4000.d3))
            n = nu1*g + nl1
*
            go to 6
*
*         * normal dense
c  20       aaa = -(alt-1417.d3)/386.3d3
   20       aaa = -(alt-1417.d3)/960.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 21
            endif
            p1 = dexp(aaa)
   21       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 2.d6*p1/p2
            nu1 = 1.d4*dexp(-7.7d0*(1.d0-1.d0/r))*
     &                     (1.d0-.99d0*dexp(-alt/4000.d3))
            n = nu1*g + nl1
*
            go to 6
c
c         * normal 2e7 +Mag.
   30       aaa = -(alt-1417.d3)/960.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 31
            endif
            p1 = dexp(aaa)
   31       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 2.d7*p1/p2
            nu1 = 1.d4*dexp(-7.7d0*(1.d0-1.d0/r))*
     &                     (1.d0-.99d0*dexp(-alt/4000.d3))
            n = nu1*g + nl1
*
            go to 6
c
c         * normal 6e5 + Mag.
   40       aaa = -(alt-1417.d3)/960.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 41
            endif
            p1 = dexp(aaa)
   41       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 6.d5*p1/p2
            nu1 = 1.d4*dexp(-7.7d0*(1.d0-1.d0/r))*
     &                     (1.d0-.99d0*dexp(-alt/4000.d3))
            n = nu1*g + nl1
*
            go to 6
c
*         * normal dense : only using ionospheric density 980103
c                         scale height is set at H=960Km (Melrose, 1991)
   50       aaa = -(alt-1417.d3)/640.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 51
            endif
            p1 = dexp(aaa)
   51       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 2.d7*p1/p2
            n = nl1
c
            goto 6
*
*         * normal thin_med : only using ionospheric density 980103
c                          scale height is set at H=960Km (Melrose, 1991)
   60       aaa = -(alt-1417.d3)/640.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 61
            endif
            p1 = dexp(aaa)
   61       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 6.d5*p1/p2
            n = nl1
c
            goto 6
*
*         * normal dense : only using ionospheric density 980103
c                          scale height is set at H=960Km (Melrose, 1991)
   70       aaa = -(alt-1417.d3)/960.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 71
            endif
            p1 = dexp(aaa)
   71       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 2.d7*p1/p2
            n = nl1
*
            goto 6
*
*         * normal thin_med : only using ionospheric density 980103
c                          scale height is set at H=960Km (Melrose, 1991)
   80       aaa = -(alt-1417.d3)/960.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 81
            endif
            p1 = dexp(aaa)
   81       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 6.d5*p1/p2
            n = nl1
c
            goto 6
*
*         * normal thin_med : only using ionospheric density 980103
c                          scale height is set at H=725Km
   90       aaa = -(alt-1417.d3)/725.d3
            if (aaa.le.-354.89d0) then
               p1=0.d0
               go to 91
            endif
            p1 = dexp(aaa)
   91       p2 = 1.d0+(991.d3/(alt-276.d3))**4.31
            nl1 = 6.d5*p1/p2
            n = nl1
*
****  1/cc -> 1/m3
    6    n = n * 1.d6
*
  999 return
      end
*
***********************************************************************
*
      subroutine maglat(x,ramm,flat,ns,ix,iy,aa0,ojc)
*
***********************************************************************
*
*     derive magnetic latitude "ramm" from geographic coordinate
*
*        ***************************************************
*        *  geographic caluculation is undergone based on  *
*        *   "offset tilted dipole model" approximation.   *
*        *                     ( refer to Bagenal(1983))   *
*        *    ramm  : magnetic foot point latitude         *
*        *    flat  : io's geographic foot point latitude  *
*        *    git65 : geographic longitude                 *
*        ***************************************************
*
         implicit real*8(a-h,o-z)
         real*8 x(3),ramm,git65,x1,y1,z1,x2,x3,y3,z3,rrr
         real*8 flat,col,st,xa,ya,za,aa0,u,t,avc,v(3)
         real*8 r,theta,fai,xxx,zzz,sur,alt,w(3),xw,xww,yw,zw
         character ojc*1
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         real*8  pols(2,0:31),polc(2,0:31),
     &           roff,mus,muc,dls,dlc,lns,lnc,tls,tlc
         common /polyco/pols,polc,roff,mus,muc,dls,dlc,lns,lnc,tls,tlc
*
         do i=1,3
            v(i)=x(i)
         enddo
*
         x1 = v(1)-roff*dlc*muc
         y1 = v(2)-roff*dlc*mus
         z1 = v(3)-roff*dls
         x2 = lnc*x1 + lns*y1
         y3 =-lns*x1 + lnc*y1
         x3 = tlc*x2 - tls*z1
         z3 = tls*x2 + tlc*z1
*
         git65 = datan2(y3,x3)/rad
         git65 = 360.d0 - git65
         if (git65.gt.360.d0) git65=git65-360.d0
*
         flat = polc(ns,0)
         t = pai/192.d0*(375.d0-git65)
         do i=1,9 ! flat(0deg-360deg) is nearly continuous at i=9 or 24 960121
            u=dble(i)
            flat=flat+pols(ns,i)*dsin(u*t)+polc(ns,i)*dcos(u*t)
         enddo
         flat=dabs(flat)
*
         rrr = dsqrt(x3*x3+y3*y3+z3*z3)
         st = dsqrt(x3*x3+y3*y3)/rrr
*
   15    col = dasin(dsqrt(aa0/rrr)*st)
*
         xa = aa0/rrr*x3
         ya = aa0/rrr*y3
         za = aa0*dcos(col)
*
         xw   = tlc*xa + tls*za
         zw   =-tls*xa + tlc*za
         xww  = lnc*xw - lns*ya
         yw   = lns*xw + lnc*ya
         w(1) = xww+roff*dlc*muc
         w(2) = yw +roff*dlc*mus
         w(3) = zw +roff*dls
*
         call axcs(w,r,theta,fai)
*
         xxx=dsin(theta*rad)*dsin(theta*rad)/arj0/arj0
         zzz=dcos(theta*rad)*dcos(theta*rad)/brj0/brj0
         sur = 1.d0/dsqrt( xxx+zzz )
         alt = r  - sur
*
         if (dabs(alt).le.1.0d0) go to 20
         aa0 = aa0 - alt
         go to 15
*
   20    ramm = 90.d0 - col/rad
         ramm=dabs(ramm)
*
      return
      end
*
************************************************************************
*
      subroutine dodndx(x,dndx,ns,ix,iy,noff,aa0,ggg,md,ojc)
*
************************************************************************
*
         real*8 x(3),dndx(3),xx(3),np,nm,dr,noff,aa0,ggg
         real*8 r,theta,fai,rat
         character ojc*1
*
         dr = .5d0
*
         xx(1) = x(1)
         xx(2) = x(2)
         xx(3) = x(3)
*
         do i=1,3
            xx(i) = x(i)+dr
            call axcs(xx,r,theta,fai)
            call dens(xx,theta,np,ns,ifl,rat,ix,iy,noff,aa0,ggg,md,ojc)
            xx(i) = x(i)-dr
            call axcs(xx,r,theta,fai)
            call dens(xx,theta,nm,ns,ifl,rat,ix,iy,noff,aa0,ggg,md,ojc)
            dndx(i) = (np-nm)/2.d0/dr
            xx(i) = x(i)
         enddo
*
      return
      end
*
***********************************************************************
*
      subroutine bkang(b,k)
*
************************************************************************
*
         real*8 b(3),k(3)
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         bb = b(1)*b(1) + b(2)*b(2) + b(3)*b(3)
         bk = b(1)*k(1) + b(2)*k(2) + b(3)*k(3)
         kk = k(1)*k(1) + k(2)*k(2) + k(3)*k(3)
*
         alfa = dacos( bk/dsqrt(bb)/dsqrt(kk) )/rad
*
      return
      end
*
************************************************************************
*
      subroutine dodgdw(dgdw)
*
***********************************************************************
*
         implicit real*8(a-h,o-z)
         real*8 dgdw,ps,pb,p1,p2
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
	!// wp2 : プラズマ角周波数
	!// wc2 : サイクロトロン角周波数

         ps = -4.d0*(wp2/w-2.d0*wp2*wp2/w)
         p1 = -wc2*wc2/w*si2*si2-2.d0*wc2/w*co2*(1.d0-wp2)*(1.d0-wp2)
     &        +4.d0*wc2*wp2/w*co2*(1.d0-wp2)
         p2 = ox*2.d0/root*p1
         pb = 4.d0*wp2/w+2.d0*wc2/w*si2+p2
*
         dgdw = -2.d0*kk*c*c/w/w/w+
     &           1.d0/bunbo/bunbo*(bunbo*ps-bunsi*pb)
*
      return
      end
*
***********************************************************************
*
      subroutine dodgdx(b,k,dndx,dbdx,dgdx)
*
***********************************************************************
*
         implicit real*8(a-h,o-z)
         real*8 b(3),k(3),dndx(3),dbdx(3,3),dgdx(3)
         real*8 dwp,dwc,dc2,ds2,ds4,drt
         real*8 p1,p2,ps,pb
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         do i=1,3
*
            dwp = pp*dndx(i)                                   !// dndxは d((w2/w)**2)/dxi の結果。
            p1  = b(1)*dbdx(1,i)+b(2)*dbdx(2,i)+b(3)*dbdx(3,i)
            dwc = 2.d0*pc*p1
            dc2 = (bb*(k(1)*dbdx(1,i)+k(2)*dbdx(2,i)+k(3)*dbdx(3,i))
     &            -bk*p1)/bb/bb/kk*2.d0*bk
            ds2 = -dc2
            ds4 = -2.d0*(1.d0-co2)*dc2
*
            p2 = 2.d0*si2*si2*wc2*dwc+wc2*wc2*ds4
     &          +4.d0*co2*(1.d0-wp2)*(1.d0-wp2)*dwc
     &          +4.d0*wc2*(1.d0-wp2)*(1.d0-wp2)*dc2
     &          -8.d0*wc2*co2*(1.d0-wp2)*dwp
            drt = .5d0/root*p2
            ps = ( 2.d0-4.d0*wp2 )*dwp
            pb = -2.d0*dwp-si2*dwc-wc2*ds2+ox*drt
*
            dgdx(i) = 1.d0/bunbo/bunbo*(bunbo*ps-bunsi*pb)
*
         enddo
*
      return
      end
*
***********************************************************************
*
      subroutine dodgdk(b,k,dgdk)
*
***********************************************************************
*
         implicit real*8(a-h,o-z)
         real*8 b(3),k(3),dgdk(3),dc2,ds2,ds4,p1
*
         real*8 bb,kk,bk,co2,si2,wp2,wc2,w,alfa,ox,bunsi,bunbo,root
         common /param/bb,kk,bk,co2,si2,wp2,wc2,w,
     &                 alfa,ox,bunsi,bunbo,root
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         do i=1,3
            dc2 = 2.d0*bk/bb/kk/kk*( b(i)*kk-bk*k(i) )
            ds2 = -dc2
            ds4 = -2.d0*(1.d0-co2)*dc2
            p1 = -wc2*ds2+ox*.5d0/root*
     &          ( wc2*wc2*ds4+4.d0*wc2*(1.d0-wp2)*(1.d0-wp2)*dc2 )
            dgdk(i) = 2.d0*c*c/w/w*k(i)-bunsi/bunbo/bunbo*p1
        enddo
*
      return
      end
*
***********************************************************************
*
* Connerney O6 model
*
      subroutine magfo6(x,b,dbdx,theta)
*
***********************************************************************
*
         implicit real*8(a-h,o-z)
         real*8  x(3),b(3),dbdx(3,3)
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         call axcs(x,r,theta,fai65)
*
         g10= 4.24202D0
         g11=-0.65929D0
c
         g20=-0.02181D0
         g21=-0.71106D0
         g22= 0.48714D0
c
         g30= 0.07565D0
         g31=-0.15493D0
         g32= 0.19775D0
         g33=-0.17958D0
c
         h11= 0.24116D0
c
         h21=-0.40304D0
         h22= 0.07179D0
c
         h31=-0.38824D0
         h32= 0.34243D0
         h33=-0.22439D0
c
         ct=dcos(theta*rad)
         st=dsin(theta*rad)
c
         cf1=dcos(fai65*rad)
         sf1=dsin(fai65*rad)
         cf2=dcos(2.d0*fai65*rad)
         sf2=dsin(2.d0*fai65*rad)
         cf3=dcos(3.d0*fai65*rad)
         sf3=dsin(3.d0*fai65*rad)
c
         ar1=(a/r)**3
         ar2=(a/r)**4
         ar3=(a/r)**5
c
         bthet1=-ar1*(-g10*st
     &              +( g11*cf1+h11*sf1)*ct)
         bfai1 =-ar1*(-g11*sf1+h11*cf1)
         br1   =2.d0*ar1*(g10*ct
     &                 +(g11*cf1+h11*sf1)*st)
c
         dbt1dt = -ar1*(-g10*ct-(g11*cf1+h11*sf1)*st)
         dbf1dt = 0.d0
         dbr1dt = 2.d0*ar1*(-g10*st+(g11*cf1+h11*sf1)*ct)
*
         dbt1df = -ar1*(-g11*sf1+h11*cf1)*ct
         dbf1df = -ar1*(-g11*cf1-h11*sf1)
         dbr1df = 2.d0*ar1*(-g11*sf1+h11*cf1)*st
*
         dbt1dr = 3.d0*ar1/r*(-g10*st+(g11*cf1+h11*sf1)*ct)
         dbf1dr = 3.d0*ar1/r*(-g11*sf1+h11*cf1)
         dbr1dr =-6.d0*ar1/r*( g10*ct+(g11*cf1+h11*sf1)*st)
*
         p20 =(3.d0*ct*ct-1.d0)/2.d0
         dp20=-3.d0*ct*st
         p21 =1.732050807d0*ct
         dp21=1.732050807d0*(2.0d0*ct*ct-1.0d0)
         p22 =0.866025403d0*st
         dp22=1.732050807d0*ct*st
*
         dp20dt=-3.d0*(ct*ct-st*st)
         dp21dt=-1.732050807d0*(4.0d0*ct*st)
         dp22dt=1.732050807d0*(ct*ct-st*st)
         p20dt =-3.d0*ct*st
         p21dt =-1.732050807d0*st
         p22dt =0.866025403d0*ct
         p21dtt =1.732050807d0*(ct*ct-st*st)
         p22dtt =0.866025403d0*2.d0*st*ct
c
         gh21=g21*cf1+h21*sf1
         gh22=g22*cf2+h22*sf2
         gh21df=-g21*sf1+h21*cf1
         gh22df=2.d0*(-g22*sf2+h22*cf2)
c
         bthet2=-ar2*(g20*dp20+gh21*dp21+gh22*dp22)
         bfai2 =-ar2*((-g21*sf1+h21*cf1)*p21
     &          +2.d0*(-g22*sf2+h22*cf2)*p22)
         br2= 3.0d0*ar2*(g20*p20+gh21*p21*st+gh22*p22*st)
c
         dbt2dt = -ar2*(g20*dp20dt+gh21*dp21dt+gh22*dp22dt)
         dbf2dt = -ar2*((-g21*sf1+h21*cf1)*p21dt
     &            +2.d0*(-g22*sf2+h22*cf2)*p22dt )
         dbr2dt = 3.d0*ar2*(g20*p20dt+gh21*p21dtt+gh22*p22dtt)
*
         dbt2df = -ar2*(dp21*gh21df+dp22*gh22df)
         dbf2df = -ar2*(p21*(-gh21)+4.d0*p22*(-gh22))
         dbr2df = 3.d0*ar2*(p21*st*gh21df+p22*st*gh22df)
*
         dbt2dr = 4.d0*ar2/r*(g20*dp20+gh21*dp21+gh22*dp22)
         dbf2dr = 4.d0*ar2/r*((-g21*sf1+h21*cf1)*p21
     &                  +2.d0*(-g22*sf2+h22*cf2)*p22)
         dbr2dr = -12.d0*ar2/r*(g20*p20+gh21*p21*st+gh22*p22*st)
*
         p30 =ct*(5.d0*ct*ct-3.d0)/2.d0
         dp30=3.d0*st*(1.d0-5.d0*ct*ct)/2.d0
         p31 =0.612372435d0*(-5.d0*st*st+4.d0)
         dp31=0.612372435d0*ct*(-15.d0*st*st+4.d0)
         p32 =1.936491673d0*ct*st
         dp32=1.936491673d0*st*(3.d0*ct*ct-1.d0)
         p33 =0.790569415d0*st*st
         dp33=2.371708245d0*st*st*ct
*
         p30dt =.5d0*(-st*(5.d0*ct*ct-3.d0)-10.d0*ct*ct*st)
         p31dt =0.612372435d0*(-10.d0*st*ct)
         p32dt =1.936491673d0*(ct*ct-st*st)
         p33dt =0.790569415d0*(2.d0*st*ct)
         dp30dt=1.5d0*(ct*(1.d0-5.d0*ct*ct)+st*(10.d0*ct*st))
         dp31dt=0.612372435d0*(15.d0*st*st*st-4.d0*st-30.d0*st*ct*ct)
         dp32dt=1.936491673d0*(3.d0*ct*ct*ct-ct-6.d0*ct*st*st)
         dp33dt=2.371708245d0*(2.d0*st*ct*ct-st*st*st)
         p31dtt=p31dt*st+p31*ct
         p32dtt=p32dt*st+p32*ct
         p33dtt=p33dt*st+p33*ct
c
         gh31=g31*cf1+h31*sf1
         gh32=g32*cf2+h32*sf2
         gh33=g33*cf3+h33*sf3
c
         gh31df=-g31*sf1+h31*cf1
         gh32df=2.d0*(-g32*sf2+h32*cf2)
         gh33df=3.d0*(-g33*sf3+h33*cf3)
c
         bthet3=-ar3*(g30*dp30+gh31*dp31+gh32*dp32+gh33*dp33)
         bfai3=-ar3*((-g31*sf1+h31*cf1)*p31
     &        +2.0d0*(-g32*sf2+h32*cf2)*p32
     &        +3.0d0*(-g33*sf3+h33*cf3)*p33)
         br3=4.0*ar3*(g30*p30+gh31*p31*st+gh32*p32*st+gh33*p33*st)
c
         dbt3dt = -ar3*(g30*dp30dt+gh31*dp31dt+gh32*dp32dt+gh33*dp33dt)
         dbf3dt = -ar3*( (-g31*sf1+h31*cf1)*p31dt
     &            +2.0d0*(-g32*sf2+h32*cf2)*p32dt
     &            +3.0d0*(-g33*sf3+h33*cf3)*p33dt )
         dbr3dt = 4.d0*ar3*(g30*p30dt+gh31*p31dtt
     &                     +gh32*p32dtt+gh33*p33dtt)
c
         dbt3df = -ar3*(gh31df*dp31+gh32df*dp32+gh33df*dp33)
         dbf3df = -ar3*(-gh31*p31-4.d0*gh32*p32-9.d0*gh33*p33)
         dbr3df = 4.d0*ar3*(gh31df*p31*st+gh32df*p32*st+gh33df*p33*st)
*
         dbt3dr = 5.d0*ar3/r*(g30*dp30+gh31*dp31+gh32*dp32+gh33*dp33)
         dbf3dr = 5.d0*ar3/r*((-g31*sf1+h31*cf1)*p31
     &                 +2.0d0*(-g32*sf2+h32*cf2)*p32
     &                 +3.0d0*(-g33*sf3+h33*cf3)*p33)
         dbr3dr =-20.d0*ar3/r*
     &            (g30*p30+gh31*p31*st+gh32*p32*st+gh33*p33*st)
c
         br     = br1+br2+br3
         btheta = bthet1+bthet2+bthet3
         bfai   = bfai1+bfai2+bfai3
*
         b(1) = st*cf1*br + ct*cf1*btheta - sf1*bfai
         b(2) = st*sf1*br + ct*sf1*btheta + cf1*bfai
         b(3) = ct    *br - st    *btheta
*
         dbrdr = dbr1dr+dbr2dr+dbr3dr
         dbtdr = dbt1dr+dbt2dr+dbt3dr
         dbfdr = dbf1dr+dbf2dr+dbf3dr
*
         dbrdt = dbr1dt+dbr2dt+dbr3dt
         dbtdt = dbt1dt+dbt2dt+dbt3dt
         dbfdt = dbf1dt+dbf2dt+dbf3dt
*
         dbrdf = dbr1df+dbr2df+dbr3df
         dbtdf = dbt1df+dbt2df+dbt3df
         dbfdf = dbf1df+dbf2df+dbf3df
*
         dbxdr = st*cf1*dbrdr + ct*cf1*dbtdr - sf1*dbfdr
         dbxdt = cf1*(ct*br+st*dbrdt) +cf1*(-st*btheta+ct*dbtdt)
     &          -sf1*dbfdt
         dbxdf = st*(-sf1*br+cf1*dbrdf)+ct*(-sf1*btheta+cf1*dbtdf)
     &          -(cf1*bfai+sf1*dbfdf)
*
         dbydr = st*sf1*dbrdr + ct*sf1*dbtdr + cf1*dbfdr
         dbydt = sf1*(ct*br+st*dbrdt)+sf1*(-st*btheta+ct*dbtdt)
     &          +cf1*dbfdt
         dbydf = st*(cf1*br+sf1*dbrdf)+ct*(cf1*btheta+sf1*dbtdf)
     &          +(-sf1*bfai+cf1*dbfdf)
*
         dbzdr = ct*dbrdr - st*dbtdr
         dbzdt =(-st*br+ct*dbrdt) - (ct*btheta+st*dbtdt)
         dbzdf = ct*dbrdf - st*dbtdf
*
         drdx = x(1)/r
         drdy = x(2)/r
         drdz = x(3)/r
*
         dtdx = x(1)*x(3)/r/r/r/st
         dtdy = x(2)*x(3)/r/r/r/st
         dtdz = -(x(1)*x(1)+x(2)*x(2))/r/r/r/st
*
         dfdx = -x(2)/x(1)/x(1)*cf1*cf1
         dfdy = cf1*cf1/x(1)
         dfdz = 0.d0
*
         dbdx(1,1) = dbxdr*drdx + dbxdt*dtdx + dbxdf*dfdx
         dbdx(1,2) = dbxdr*drdy + dbxdt*dtdy + dbxdf*dfdy
         dbdx(1,3) = dbxdr*drdz + dbxdt*dtdz + dbxdf*dfdz
*
         dbdx(2,1) = dbydr*drdx + dbydt*dtdx + dbydf*dfdx
         dbdx(2,2) = dbydr*drdy + dbydt*dtdy + dbydf*dfdy
         dbdx(2,3) = dbydr*drdz + dbydt*dtdz + dbydf*dfdz
*
         dbdx(3,1) = dbzdr*drdx + dbzdt*dtdx + dbzdf*dfdx
         dbdx(3,2) = dbzdr*drdy + dbzdt*dtdy + dbzdf*dfdy
         dbdx(3,3) = dbzdr*drdz + dbzdt*dtdz + dbzdf*dfdz
*
*        unit convert : gauss->tesla
*
         do i=1,3
            do j=1,3
               dbdx(i,j) = dbdx(i,j)*1.d-4
            enddo
            b(i) = b(i)*1.d-4
         enddo
*
      return
      end
*
***********************************************************************
*
      subroutine jfc(v,cml)
*
***********************************************************************
*
         real*8 v(3),vx,vy,vz
         real*8 cml
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         vx = v(1)
         vy = v(2)
         vz = v(3)
*
         v(1) = dcos(cml*rad)*vx - dsin(cml*rad)*vy
         v(2) = dsin(cml*rad)*vx + dcos(cml*rad)*vy
         v(3) = vz
*
      return
      end
*
***********************************************************************
*
      subroutine jec(v,de)
*
***********************************************************************
*
         real*8 v(3),vx,vy,vz
         real*8 de
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         vx = v(1)
         vy = v(2)
         vz = v(3)
*
         v(1) = dcos(de*rad)*vx + dsin(de*rad)*vz
         v(2) = vy
         v(3) =-dsin(de*rad)*vx + dcos(de*rad)*vz
*
      return
      end
*
***********************************************************************
*
      subroutine axcs(v,r,theta,fai)
*
***********************************************************************
*
         real*8 v(3),r,theta,fai
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         r = dsqrt(v(1)*v(1)+v(2)*v(2)+v(3)*v(3))
         theta = dacos(v(3)/r)/rad
         fai = dacos(v(1)/dsqrt(v(1)*v(1)+v(2)*v(2)))/rad
         if (v(2).lt.0.d0) fai=-fai
*
      return
      end
*
***********************************************************************
*
      subroutine axsc(r,theta,fai,v)
*
***********************************************************************
*
         real*8 v(3),r,theta,fai
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         v(1) = r*dsin(theta*rad)*dcos(fai*rad)
         v(2) = r*dsin(theta*rad)*dsin(fai*rad)
         v(3) = r*dcos(theta*rad)
*
      return
      end
*
***********************************************************************
*
      subroutine cutoff(tf,n,ox,e,ep,me,fcut,rt)
*
***********************************************************************
*
         real*8 tf,n,fc,fp,fcut,e,ep,me,ox,rt
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         integer sw1,sw2
         common /switch/sw1,sw2
*
         fp = dsqrt(e*e*n/ep/me)/2.d0/pai
         fc = e*tf/me/2.d0/pai
*
         if ( ox.eq.-1.d0 ) go to 10
*
*   * l-o mode cutoff : plasma freq.
         fcut = fp
         go to 20
*   * r-x mode cutoff
   10    fcut = fc/2.d0+dsqrt(fp*fp+fc*fc/4.d0)
*
   20 return
      end
*
**********************************************************************
*
      subroutine polset(ojc)
*
**********************************************************************
*
      character ojc*1
      real*8  mu,dl,lon,tlt
      real*8  pols(2,0:31),polc(2,0:31),
     &        roff,mus,muc,dls,dlc,lns,lnc,tls,tlc
      common /polyco/pols,polc,roff,mus,muc,dls,dlc,lns,lnc,tls,tlc
      real*8  pai,rad,pp,pc,c,a,arj0,brj0
      common /const/pai,rad,pp,pc,c,a,arj0,brj0
c
         if (ojc.eq.'c') open (21,file='ffto6.dat',status='old')
         if (ojc.eq.'v') open (21,file='fftv4.dat',status='old')
c
	    do i=1,2
	       read(21,*)
	       do j=0,31
		  read(21,*) polc(i,j),pols(i,j)
               enddo
            enddo
c
            read(21,*)
            read(21,*) roff,mu,dl,lon,tlt
c
         close(21)
c
         roff = roff*a
         mus  = dsin(mu*rad)
         muc  = dcos(mu*rad)
         dls  = dsin(dl*rad)
         dlc  = dcos(dl*rad)
         lns  = dsin(lon*rad)
         lnc  = dcos(lon*rad)
         tls  = dsin(tlt*rad)
         tlc  = dcos(tlt*rad)
*
      return
      end
*
***********************************************************************
*
cc VIP4 Jovian Magnetic Field Model, released by Connerney, 1998
*
      subroutine jfdv4(r,theta,fai,br,bt,bf,tf)
*
***********************************************************************
*
         implicit real*8 (a-h,o-z)
         real*8 g(4,0:4),h(4,0:4),dpnm(4,0:4),pnm(4,0:4)
         real*8 cf(0:6),sf(0:6),ar(0:6),lf,dlf
c
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
c
         nmx=4
c
         t=theta*rad
         f=fai*rad
c
         g( 1, 0)= 420500.d-5
         g( 1, 1)=- 65900.d-5 
         g( 2, 0)=-  5100.d-5
         g( 2, 1)=- 61900.d-5
         g( 2, 2)=  49700.d-5
         g( 3, 0)=-  1600.d-5
         g( 3, 1)=- 52000.d-5
         g( 3, 2)=  24400.d-5
         g( 3, 3)=- 17600.d-5
         g( 4, 0)=- 16800.d-5
         g( 4, 1)=  22200.d-5
         g( 4, 2)=-  6100.d-5
         g( 4, 3)=- 20200.d-5
         g( 4, 4)=   6600.d-5
         h( 1, 1)=  25000.d-5
         h( 2, 1)=- 36100.d-5
         h( 2, 2)=   5300.d-5
         h( 3, 1)=-  8800.d-5
         h( 3, 2)=  40800.d-5
         h( 3, 3)=- 31600.d-5
         h( 4, 1)=   7600.d-5
         h( 4, 2)=  40400.d-5
         h( 4, 3)=- 16600.d-5
         h( 4, 4)=   3900.d-5
c
         do i=0,nmx+2
            ar(i)=(a/r)**i
            cf(i)=dcos(i*f)
            sf(i)=dsin(i*f)
         enddo
c
cc define Pnm & d(Pnm(cos(theta)))/d(theta)
         do n=1,nmx
            do m=0,n
               pnm(n,m)=lf(n,m,t)
               dpnm(n,m)=dlf(n,m,t)
            enddo
         enddo
c
         br=0.d0
         bt=0.d0
         bf=0.d0
         do n=1,nmx
            sr=0.d0
            st=0.d0
            sh=0.d0
            do m=0,n
              sr=sr+(n+1)*ar(n+2)*(g(n,m)*cf(m)+h(n,m)*sf(m))* pnm(n,m)
              st=st-      ar(n+2)*(g(n,m)*cf(m)+h(n,m)*sf(m))*dpnm(n,m)
              sh=sh+    m*ar(n+2)*(g(n,m)*sf(m)-h(n,m)*cf(m))* pnm(n,m)
            enddo
            br=br+sr
            bt=bt+st
            bf=bf+sh
         enddo
         bf=bf/dsin(t)
c
         tf=dsqrt(br*br+bt*bt+bf*bf) ![G]
c
cc Unit convert : G->Tesra
         br=br*1.d-4
         bt=bt*1.d-4
         bf=bf*1.d-4
         tf=tf*1.d-4
c
      return
      end
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c Associated Legendre function with Schmidt normalization
c
      real*8 function lf(n,m,t)
c
         real*8 t
c
         no=n*(n+1)/2+m
         goto (10,11,20,21,22,30,31,32,33,40,41,42,43,44) no
c
   10    lf=dcos(t)
            goto 999
   11    lf=dsin(t)
            goto 999
   20    lf=.5d0*(3.d0*dcos(t)**2-1.d0)
            goto 999
   21    lf=3.d0*dcos(t)*sin(t)
            goto 999
   22    lf=3.d0*dsin(t)**2
            goto 999
   30    lf=.5d0*(5.d0*dcos(t)**3-3.d0*dcos(t))
            goto 999
   31    lf=1.5d0*dsin(t)*(5.d0*dcos(t)**2-1.d0)
            goto 999
   32    lf=15.d0*dcos(t)*dsin(t)*dsin(t)
            goto 999
   33    lf=15.d0*dsin(t)**3
            goto 999
   40    lf=1.d0/64.d0*(35.d0*dcos(4.d0*t)+20.d0*dcos(2.d0*t)+9.d0)
            goto 999
   41    lf=5.d0/16.d0*(2.d0*dsin(2.d0*t)+7.d0*dsin(4.d0*t))
            goto 999
   42    lf=15.d0/16.d0*(3.d0+4.d0*dcos(2.d0*t)-7.d0*dcos(4.d0*t))
            goto 999
   43    lf=105.d0/8.d0*(2.d0*dsin(2.d0*t)-dsin(4.d0*t))
            goto 999
   44    lf=105.d0*dsin(t)**4
c
cc Schmidt normalization (for Geophysics)
c
  999    if (m.ne.0) lf=dsqrt(2.d0*dble(kai(n-m))/dble(kai(n+m)))*lf
c
      return
      end
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
cc kaijyo estimation
c
      integer function kai(i)
c
         kai=1
         if (i.ge.2) then
            do j=1,i
               kai=kai*j
            enddo
         endif
      return
      end
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c Derivative of Associated Legendre function with Schmidt normalization
c
      real*8 function dlf(n,m,t)
c
         real*8 t
c
         no=n*(n+1)/2+m
         goto (10,11,20,21,22,30,31,32,33,40,41,42,43,44) no
c
   10    dlf=-dsin(t)
            goto 999
   11    dlf=dcos(t)
            goto 999
   20    dlf=-3.d0*dsin(t)*dcos(t)
            goto 999
   21    dlf=3.d0*dcos(2.d0*t)
            goto 999
   22    dlf=6.d0*dsin(t)*dcos(t)
            goto 999
   30    dlf=-7.5d0*dcos(t)**2*dsin(t)+1.5d0*dsin(t)
            goto 999
   31    dlf=7.5d0*dcos(t)**3-15.d0*dcos(t)*dsin(t)**2-1.5d0*dcos(t)
            goto 999
   32    dlf=15.d0*(-dsin(t)**3+2.d0*dsin(t)*dcos(t)**2)
            goto 999
   33    dlf=45.d0*dsin(t)**2*dcos(t)
            goto 999
   40    dlf=-5.d0/16.d0*(7.d0*dsin(4.d0*t)+2.d0*dsin(2.d0*t))
            goto 999
   41    dlf=5.d0/4.d0*(dcos(2.d0*t)+7.d0*dcos(4.d0*t))
            goto 999
   42    dlf=15.d0/4.d0*(-2.d0*dsin(2.d0*t)+7.d0*dsin(4.d0*t))
            goto 999
   43    dlf=105.d0/2.d0*(dcos(2.d0*t)-dcos(4.d0*t))
            goto 999
   44    dlf=420.d0*dsin(t)**3*dcos(t)
c
cc Schmidt normalization (for Geophysics)
c
  999    if (m.ne.0) dlf=dsqrt(2.d0*dble(kai(n-m))/dble(kai(n+m)))*dlf
c
      return
      end
*
***********************************************************************
*
* dB/dr : Connerney VIP4 model
*
      subroutine magfv4(x0,b0,dbdx,theta)
*
***********************************************************************
*
         implicit real*8(a-h,o-z)
         real*8  x0(3),b0(3),dbdx(3,3)
         real*8  x1(3),x2(3),b1(3),b2(3)
*
         real*8  pai,rad,pp,pc,c,a,arj0,brj0
         common /const/pai,rad,pp,pc,c,a,arj0,brj0
*
         dd=1.d-4 !displacemnt length for calculating derivatives
c
         call axcs(x0,r,theta,fai)
         call jfdv4(r,theta,fai,br,bt,bf,tf)
         call axsc(tf,theta,fai,b0)
c
         do i=1,3 !x,y,z
            do k=1,3
               x1(k)=x0(k)
               x2(k)=x0(k)
            enddo
            x1(i)=x0(i)-dd/2.d0
            x2(i)=x0(i)+dd/2.d0
            call axcs(x1,r,theta,fai)
            call jfdv4(r,theta,fai,br,bt,bf,tf)
            call axsc(tf,theta,fai,b1)
            call axcs(x2,r,theta,fai)
            call jfdv4(r,theta,fai,br,bt,bf,tf)
            call axsc(tf,theta,fai,b2)
            do j=1,3 !Bx,By,Bz
               dbdx(j,i)=(b2(j)-b1(j))/dd
c              write(6,*) j,i,dbdx(j,i)
c              if (dbdx(j,i).eq.0.) write(6,*) j,i,dbdx(j,i)
            enddo
         enddo
c
      return
      end

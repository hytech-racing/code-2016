
# Makefile for gertboard test programs
#
#
# Copyright (C) Gert Jan van Loo & Myra VanInwegen 2012
# No rights reserved
# You may treat this program as if it was in the public domain
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Keep things simple for novice users
# so do not use any implicit rules!
#
all : test_can 

clean :
	rm -r *.o
	rm -r test_can

test_can: lib.o candump_lib.o test_can.o
	gcc -o test_can test_can.o

# The next lines generate the various object files

lib.o : lib.c 
	gcc -c lib.c

candump_lib.o : candump_lib.c
	gcc -c candump_lib.c

test_can.o : test_can.c
	gcc -c test_can.c

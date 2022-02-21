// Karbo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Karbo.  If not, see <http://www.gnu.org/licenses/>.
#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <streambuf>

namespace platform_system {
class SocketStreambuf: public std::streambuf {
  public:
    SocketStreambuf(char *data, size_t lenght);
    ~SocketStreambuf();
    void getRespdata(std::vector<uint8_t> &data);
    void setRespdata(const std::vector<uint8_t> &data);

  private:
    size_t lenght;
    bool read_t;
    std::array<uint8_t, 1024> writeBuf;
    std::vector<uint8_t> readBuf;
    std::vector<uint8_t> resp_data;
    std::streambuf::int_type overflow(std::streambuf::int_type ch) override;
    std::streambuf::int_type underflow() override;
    int sync() override;
    bool dumpBuffer(bool finalize);
};
}
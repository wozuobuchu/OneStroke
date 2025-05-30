#ifndef _HEAD_DFS_HPP
#define _HEAD_DFS_HPP

namespace graph_algo {

/**
* @brief ARGS :
* @brief #pair<int,int> start       START_POINT
* @brief #pair<int,int> end           END_POINT
* @brief #MatrixT&& graph            MOVE_GRAPH
*/
class DfsHamiltonPath final {
private:
    std::vector<std::vector<char>> _matrix;
    std::vector<std::vector<bool>> _visited;
    std::deque<std::pair<int,int>> _path;
    std::deque<std::pair<int,int>> _path_copy;
    
    std::vector<std::vector<int>> _direction;

    int _node_num;

    bool _find_ans;

    void dfs(int sx,int sy) {
        auto lam_dfs = [this](auto&& lam_dfs, int cur_x, int cur_y, int step) -> void {
            if(_find_ans) return;
            // check edge
            auto [edge_x, edge_y] = _size;
            if(!(0<=cur_x && cur_x<edge_x && 0<=cur_y && cur_y<edge_y)) return;
            // check visited
            if(_visited[cur_x][cur_y]) return;
            _visited[cur_x][cur_y] = true;
            // check end
            auto [end_x,end_y] = _end;
            if(cur_x==end_x && cur_y==end_y && step>=_node_num) {
                _find_ans = true;
                _path_copy.assign(_path.begin(),_path.end());
                return;
            }
            // trim branch
            int manhattan = abs(cur_x - end_x) + abs(cur_y - end_y);
            int remain = _node_num - step;
            if ((remain - manhattan) % 2 != 0) {
                _visited[cur_x][cur_y] = false;
                return;
            }
            // search nearby
            for(int i=0; i<4; ++i) {
                std::pair<int,int> tar_pos{cur_x+_direction[i][0], cur_y+_direction[i][1]};
                auto [tar_x,tar_y] = tar_pos;

                if(_find_ans) return;
                if(!(0<=tar_x && tar_x<edge_x && 0<=tar_y && tar_y<edge_y)) continue;
                if(_matrix[tar_x][tar_y]) continue;
                if(_visited[tar_x][tar_y]) continue;

                _path.push_back({tar_x, tar_y});
                lam_dfs(lam_dfs, tar_x, tar_y, step+1);
                _path.pop_back();
            }
            _visited[cur_x][cur_y] = false;
        };

        lam_dfs(lam_dfs,sx,sy,1);

        return;
    }
public:
    std::pair<int, int> _start;
    std::pair<int, int> _end;
    std::pair<int, int> _size;

    DfsHamiltonPath() = delete;
    DfsHamiltonPath(const DfsHamiltonPath&) = delete;
    DfsHamiltonPath& operator=(const DfsHamiltonPath&) = delete;

    template <typename MatrixT>
    explicit DfsHamiltonPath(std::pair<int,int> start, std::pair<int,int> end, MatrixT&& graph) : 
        _direction(std::vector<std::vector<int>>{{1,0},{0,1},{-1,0},{0,-1}}),
        _find_ans(false),
        _start(start),
        _end(end),
        _matrix(std::forward<MatrixT>(graph)),
        _size({_matrix.size(), _matrix[0].size()}),
        _node_num(0)
    {
        _visited.resize(_size.first, std::vector<bool>(_size.second, false));

        for(auto& v : _matrix) for(auto& c : v) if(!c) ++_node_num;

        auto [sx,sy] = _start;
        _path.push_front(_start);
        dfs(sx,sy);

    }

    ~DfsHamiltonPath(){}

    bool isAnsFind() const {
        return _find_ans;
    }

    int getNodeNum() const { return this->_node_num; }

    std::deque<std::pair<int,int>> getSolution(){ return this->_path_copy; }

    std::vector<std::vector<char>> getOriginGrid() { return this->_matrix; }

};

}

#endif
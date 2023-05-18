﻿#pragma once

#include <csim/init_ops.hpp>
#include <csim/memory_ops.hpp>
#include <csim/stat_ops.hpp>
#include <csim/update_ops.hpp>
#include <iostream>
#include <vector>

#include "csim/MPIutil.hpp"
#include "exception.hpp"
#include "type.hpp"
#include "utility.hpp"

/**
 * \~japanese-en 量子状態の基底クラス
 */
class QuantumStateBase {
protected:
    ITYPE _dim;
    UINT _qubit_count;
    UINT _inner_qc;
    UINT _outer_qc;
    bool _is_state_vector;
    std::vector<UINT> _classical_register;
    UINT _device_number;
    void* _cuda_stream;

public:
    const UINT& qubit_count; /**< \~japanese-en 量子ビット数 */
    const UINT& inner_qc; /**< \~japanese-en ノード内量子ビット数 */
    const UINT& outer_qc; /**< \~japanese-en ノード外量子ビット数 */
    const ITYPE& dim;     /**< \~japanese-en 量子状態の次元 */
    const std::vector<UINT>&
        classical_register; /**< \~japanese-en 古典ビットのレジスタ */
    const UINT& device_number;
    /**
     * \~japanese-en コンストラクタ
     *
     * @param qubit_count_ 量子ビット数
     */
    QuantumStateBase(UINT qubit_count_, bool is_state_vector);

    // 本来、QuantumStateBase(UINT, bool, bool) とすべきだが、
    // QuantumStateBase(UINT, bool, UINT) に対し ambiguous error が発生するため
    // QuantumStateBase(UINT, bool, int) とした
    QuantumStateBase(
        UINT qubit_count_, bool is_state_vector, int use_multi_cpu);

    QuantumStateBase(
        UINT qubit_count_, bool is_state_vector, UINT device_number_);
    /**
     * \~japanese-en デストラクタ
     */
    virtual ~QuantumStateBase();

    /**
     * \~japanese-en 量子状態が状態ベクトルか密度行列かを判定する
     */
    virtual bool is_state_vector() const;

    /**
     * \~japanese-en 量子状態を計算基底の0状態に初期化する
     */
    virtual void set_zero_state() = 0;

    /**
     * \~japanese-en ノルム0の状態 (要素がすべて0のベクトル) にする
     */
    virtual void set_zero_norm_state() = 0;

    /**
     * \~japanese-en 量子状態を<code>comp_basis</code>の基底状態に初期化する
     *
     * @param comp_basis 初期化する基底を表す整数
     */
    virtual void set_computational_basis(ITYPE comp_basis) = 0;

    /**
     * \~japanese-en 量子状態をHaar
     * randomにサンプリングされた量子状態に初期化する
     */
    virtual void set_Haar_random_state() = 0;

    /**
     * \~japanese-en 量子状態をシードを用いてHaar
     * randomにサンプリングされた量子状態に初期化する
     */
    virtual void set_Haar_random_state(UINT seed) = 0;

    /**
     * \~japanese-en
     * <code>target_qubit_index</code>の添え字の量子ビットを測定した時、0が観測される確率を計算する。
     *
     * 量子状態は変更しない。
     * @param target_qubit_index
     * @return double
     */
    virtual double get_zero_probability(UINT target_qubit_index) const = 0;

    /**
     * \~japanese-en 複数の量子ビットを測定した時の周辺確率を計算する
     *
     * @param measured_values
     * 量子ビット数と同じ長さの0,1,2の配列。0,1はその値が観測され、2は測定をしないことを表す。
     * @return 計算された周辺確率
     */
    virtual double get_marginal_probability(
        std::vector<UINT> measured_values) const = 0;

    /**
     * \~japanese-en
     * 計算基底で測定した時得られる確率分布のエントロピーを計算する。
     *
     * @return エントロピー
     */
    virtual double get_entropy() const = 0;

    /**
     * \~japanese-en 量子状態のノルムを計算する
     *
     * 量子状態のノルムは非ユニタリなゲートを作用した時に小さくなる。
     * @return ノルム
     */
    virtual double get_squared_norm() const = 0;

    /**
     * \~japanese-en 量子状態のノルムを計算する
     *
     * 量子状態のノルムは非ユニタリなゲートを作用した時に小さくなる。
     * @return ノルム
     */
    virtual double get_squared_norm_single_thread() const = 0;

    /**
     * \~japanese-en 量子状態を正規化する
     *
     * @param norm 自身のノルム
     */
    virtual void normalize(double squared_norm) = 0;

    /**
     * \~japanese-en 量子状態を正規化する
     *
     * @param norm 自身のノルム
     */
    virtual void normalize_single_thread(double squared_norm) = 0;

    /**
     * \~japanese-en バッファとして同じサイズの量子状態を作成する。
     *
     * @return 生成された量子状態
     */
    virtual QuantumStateBase* allocate_buffer() const = 0;

    /**
     * \~japanese-en 自身の状態のディープコピーを生成する
     *
     * @return 自身のディープコピー
     */
    virtual QuantumStateBase* copy() const = 0;

    /**
     * \~japanese-en <code>state</code>の量子状態を自身へコピーする。
     */
    virtual void load(const QuantumStateBase* state) = 0;

    /**
     * \~japanese-en <code>state</code>の量子状態を自身へコピーする。
     */
    virtual void load(const std::vector<CPPCTYPE>& state) = 0;

    /**
     * \~japanese-en <code>state</code>の量子状態を自身へコピーする。
     */
    virtual void load(const CPPCTYPE* state) = 0;

    /**
     * \~japanese-en
     * 量子状態が配置されているメモリを保持するデバイス名を取得する。
     */
    virtual const std::string get_device_name() const = 0;

    virtual void* data() const = 0;

    /**
     * \~japanese-en
     * 量子状態をC++の<code>std::complex\<double\></code>の配列として取得する
     *
     * @return 複素ベクトルのポインタ
     */
    virtual CPPCTYPE* data_cpp() const = 0;

    /**
     * \~japanese-en 量子状態をcsimのComplex型の配列として取得する
     *
     * @return 複素ベクトルのポインタ
     */
    virtual CTYPE* data_c() const = 0;

    /**
     * \~japanese-en
     * 量子状態をC++の<code>std::complex\<double\></code>の配列として新たに確保する
     *
     * @return 複素ベクトルのポインタ
     */
    virtual CPPCTYPE* duplicate_data_cpp() const = 0;

    /**
     * \~japanese-en 量子状態をcsimのComplex型の配列として新たに確保する。
     *
     * @return 複素ベクトルのポインタ
     */
    virtual CTYPE* duplicate_data_c() const = 0;

    /**
     * \~japanese-en 量子状態を足しこむ
     */
    virtual void add_state(const QuantumStateBase* state) = 0;

    /**
     * \~japanese-en 量子状態を係数付きで足しこむ
     */
    virtual void add_state_with_coef(
        CPPCTYPE coef, const QuantumStateBase* state) = 0;

    /**
     * \~japanese-en 量子状態を係数付きで足しこむ
     */
    virtual void add_state_with_coef_single_thread(
        CPPCTYPE coef, const QuantumStateBase* state) = 0;

    /**
     * \~japanese-en 複素数をかける
     */
    virtual void multiply_coef(CPPCTYPE coef) = 0;

    virtual void multiply_elementwise_function(
        const std::function<CPPCTYPE(ITYPE)>& func) = 0;

    /**
     * \~japanese-en 指定した添え字の古典レジスタの値を取得する。
     *
     * @param index セットするレジスタの添え字
     * @return 古典レジスタの値
     */
    virtual UINT get_classical_value(UINT index);

    /**
     * \~japanese-en 指定した添え字の古典レジスタに値をセットする
     *
     * @param index セットするレジスタの添え字
     * @param val セットする値
     */
    virtual void set_classical_value(UINT index, UINT val);

    /**
     * \~japanese-en 古典レジスタのベクトルを返す
     *
     * @return 古典レジスタ
     */
    virtual const std::vector<UINT> get_classical_register() const;

    /**
     * \~japanese-en 量子状態を測定した際の計算基底のサンプリングを行う
     *
     * @param[in] sampling_count サンプリングを行う回数
     * @param[in] random_seed サンプリングで乱数を振るシード値
     * @return サンプルされた値のリスト
     */
    virtual std::vector<ITYPE> sampling(UINT sampling_count) = 0;
    virtual std::vector<ITYPE> sampling(
        UINT sampling_count, UINT random_seed) = 0;

    /**
     * \~japanese-en property treeに変換
     *
     * @return ptree
     */
    virtual boost::property_tree::ptree to_ptree() const = 0;

    /**
     * \~japanese-en 量子回路のデバッグ情報の文字列を生成する
     *
     * @return 生成した文字列
     */
    virtual std::string to_string() const;

    /**
     * \~japanese-en 量子状態のデバッグ情報を出力する。
     *
     * @return 受け取ったストリーム
     */
    friend std::ostream& operator<<(
        std::ostream& os, const QuantumStateBase& state);

    /**
     * \~japanese-en 量子状態のデバッグ情報を出力する。
     *
     * @return 受け取ったストリーム
     */
    friend std::ostream& operator<<(
        std::ostream& os, const QuantumStateBase* state);

    virtual void* get_cuda_stream() const;
};

class QuantumStateCpu : public QuantumStateBase {
private:
    CPPCTYPE* _state_vector;
    Random random;

public:
    /**
     * \~japanese-en コンストラクタ
     *
     * @param qubit_count_ 量子ビット数
     */
    explicit QuantumStateCpu(UINT qubit_count_);

    /**
     * \~japanese-en コンストラクタ
     *
     * @param qubit_count_ 量子ビット数
     * @param use_multi_cpu Flag to use multi CPUs
     */
    explicit QuantumStateCpu(UINT qubit_count_, bool use_multi_cpu);

    /**
     * \~japanese-en デストラクタ
     */
    virtual ~QuantumStateCpu();

    /**
     * \~japanese-en 量子状態を計算基底の0状態に初期化する
     */
    virtual void set_zero_state() override;

    /**
     * \~japanese-en 量子状態をノルム0の状態にする
     */
    virtual void set_zero_norm_state() override;

    /**
     * \~japanese-en 量子状態を<code>comp_basis</code>の基底状態に初期化する
     *
     * @param comp_basis 初期化する基底を表す整数
     */
    virtual void set_computational_basis(ITYPE comp_basis) override;
    /**
     * \~japanese-en 量子状態をHaar
     * randomにサンプリングされた量子状態に初期化する
     */
    virtual void set_Haar_random_state() override;
    /**
     * \~japanese-en 量子状態をシードを用いてHaar
     * randomにサンプリングされた量子状態に初期化する
     */
    virtual void set_Haar_random_state(UINT seed) override;
    /**
     * \~japanese-en
     * <code>target_qubit_index</code>の添え字の量子ビットを測定した時、0が観測される確率を計算する。
     *
     * 量子状態は変更しない。
     * @param target_qubit_index
     * @return double
     */
    virtual double get_zero_probability(UINT target_qubit_index) const;
    /**
     * \~japanese-en 複数の量子ビットを測定した時の周辺確率を計算する
     *
     * @param measured_values
     * 量子ビット数と同じ長さの0,1,2の配列。0,1はその値が観測され、2は測定をしないことを表す。
     * @return 計算された周辺確率
     */
    virtual double get_marginal_probability(
        std::vector<UINT> measured_values) const override;
    /**
     * \~japanese-en
     * 計算基底で測定した時得られる確率分布のエントロピーを計算する。
     *
     * @return エントロピー
     */
    virtual double get_entropy() const override;

    /**
     * \~japanese-en 量子状態のノルムを計算する
     *
     * 量子状態のノルムは非ユニタリなゲートを作用した時に小さくなる。
     * @return ノルム
     */
    virtual double get_squared_norm() const override;

    /**
     * \~japanese-en 量子状態のノルムを計算する
     *
     * 量子状態のノルムは非ユニタリなゲートを作用した時に小さくなる。
     * @return ノルム
     */
    virtual double get_squared_norm_single_thread() const override;

    /**
     * \~japanese-en 量子状態を正規化する
     *
     * @param norm 自身のノルム
     */
    virtual void normalize(double squared_norm) override;

    /**
     * \~japanese-en 量子状態を正規化する
     *
     * @param norm 自身のノルム
     */
    virtual void normalize_single_thread(double squared_norm) override;

    /**
     * \~japanese-en バッファとして同じサイズの量子状態を作成する。
     *
     * @return 生成された量子状態
     */
    virtual QuantumStateCpu* allocate_buffer() const override;

    /**
     * \~japanese-en 自身の状態のディープコピーを生成する
     *
     * @return 自身のディープコピー
     */
    virtual QuantumStateCpu* copy() const override;
    /**
     * \~japanese-en <code>state</code>の量子状態を自身へコピーする。
     */
    virtual void load(const QuantumStateBase* _state) override;
    /**
     * \~japanese-en <code>state</code>の量子状態を自身へコピーする。
     */
    virtual void load(const std::vector<CPPCTYPE>& _state) override;

    /**
     * \~japanese-en <code>state</code>の量子状態を自身へコピーする。
     */
    virtual void load(const CPPCTYPE* _state) override;

    /**
     * \~japanese-en
     * 量子状態が配置されているメモリを保持するデバイス名を取得する。
     */
    virtual const std::string get_device_name() const override;

    /**
     * \~japanese-en 量子状態のポインタをvoid*型として返す
     */
    virtual void* data() const override;

    /**
     * \~japanese-en
     * 量子状態をC++の<code>std::complex\<double\></code>の配列として取得する
     *
     * @return 複素ベクトルのポインタ
     */
    virtual CPPCTYPE* data_cpp() const override;

    /**
     * \~japanese-en 量子状態をcsimのComplex型の配列として取得する
     *
     * @return 複素ベクトルのポインタ
     */
    virtual CTYPE* data_c() const override;

    virtual CTYPE* duplicate_data_c() const override;

    virtual CPPCTYPE* duplicate_data_cpp() const override;

    /**
     * \~japanese-en 量子状態を足しこむ
     */
    virtual void add_state(const QuantumStateBase* state) override;

    /**
     * \~japanese-en 量子状態を足しこむ
     */
    virtual void add_state_with_coef(
        CPPCTYPE coef, const QuantumStateBase* state) override;

    /**
     * \~japanese-en 量子状態を足しこむ
     */
    virtual void add_state_with_coef_single_thread(
        CPPCTYPE coef, const QuantumStateBase* state) override;

    /**
     * \~japanese-en 複素数をかける
     */
    virtual void multiply_coef(CPPCTYPE coef) override;

    virtual void multiply_elementwise_function(
        const std::function<CPPCTYPE(ITYPE)>& func) override;

    /**
     * \~japanese-en 量子状態を測定した際の計算基底のサンプリングを行う
     *
     * @param[in] sampling_count サンプリングを行う回数
     * @return サンプルされた値のリスト
     */
    virtual std::vector<ITYPE> sampling(UINT sampling_count) override;

    virtual std::vector<ITYPE> sampling(
        UINT sampling_count, UINT random_seed) override;

    virtual boost::property_tree::ptree to_ptree() const override;
};

using QuantumState = QuantumStateCpu;

namespace state {
/**
 * \~japanese-en 量子状態間の内積を計算する
 *
 * @param[in] state_bra 内積のブラ側の量子状態
 * @param[in] state_ket 内積のケット側の量子状態
 * @return 内積の値
 */
CPPCTYPE DllExport inner_product(
    const QuantumState* state_bra, const QuantumState* state_ket);
DllExport QuantumState* tensor_product(
    const QuantumState* state_left, const QuantumState* state_right);
DllExport QuantumState* permutate_qubit(
    const QuantumState* state, std::vector<UINT> qubit_order);
DllExport QuantumState* drop_qubit(const QuantumState* state,
    std::vector<UINT> target, std::vector<UINT> projection);
// create superposition of states of coef1|state1>+coef2|state2>
DllExport QuantumState* make_superposition(CPPCTYPE coef1,
    const QuantumState* state1, CPPCTYPE coef2, const QuantumState* state2);
}  // namespace state

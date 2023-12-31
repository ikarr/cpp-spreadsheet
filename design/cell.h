#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
	
	std::vector<Position> GetReferencedCells() const override;
    const std::vector<Position>& GetStoredReferencedCells() const;

    void ClearCache();
    bool HasCache() const;

private:
	class Impl {
    public:
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
		virtual std::vector<Position> GetReferencedCells() const {
            return {};
        }
        
        virtual ~Impl() = default;
    };
    
    class EmptyImpl : public Impl {
    public:
        CellInterface::Value GetValue() const override {
            return 0.0;
        }
		
        std::string GetText() const override {
            return {};
        }
    };
    
    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text)
			: text_{std::move(text)} {}
		
        CellInterface::Value GetValue() const override {
            return text_.length() > 0 && (text_[0] == '\'' ? text_.substr(1) : text_);
        }
		
        std::string GetText() const override {
            return text_;
        }
		
    private:
        std::string text_;
    };
    
    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string text, SheetInterface& sheet)
			: formula_{ParseFormula(std::move(text))}, sheet_{sheet} {}
			
        CellInterface::Value GetValue() const override {
            FormulaInterface::Value val = formula_->Evaluate(sheet_);
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            }
            return std::get<FormulaError>(val);
        }
		
        std::string GetText() const override {
            return '=' + formula_->GetExpression();
        }

        std::vector<Position> GetReferencedCells() const override {
            return formula_->GetReferencedCells();
        }

    private:
        std::unique_ptr<FormulaInterface> formula_;
        const SheetInterface& sheet_;
    };
    
    std::unique_ptr<Impl> impl_;
	SheetInterface& sheet_;
    std::vector<Position> cell_refs_;
    mutable std::unique_ptr<Value> cache_;
};
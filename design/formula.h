#pragma once

#include "common.h"

#include "FormulaAST.h"

#include <memory>
#include <variant>
#include <vector>
#include <sstream>

// Формула, позволяющая вычислять и обновлять арифметическое выражение
class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    virtual Value Evaluate(const SheetInterface& sheet) const = 0;

    virtual std::string GetExpression() const = 0;
	
	// Возвращает список ячеек, задействованных в вычислении формулы.
	// Список отсортирован по возрастанию и не содержит повторяющихся ячеек
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

// Парсит переданное выражение и возвращает объект формулы.
// Бросает FormulaException в случае если формула синтаксически некорректна.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);
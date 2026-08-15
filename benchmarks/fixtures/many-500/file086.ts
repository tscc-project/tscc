interface Item86 { id: number; name: string; }
type Key86 = string | number;
export function make86(id: number, name: string): Item86 {
  const value: Item86 = {id, name};
  return value as Item86;
}
export const item86: Item86 = make86(86, "item-86");

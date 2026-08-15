interface Item79 { id: number; name: string; }
type Key79 = string | number;
export function make79(id: number, name: string): Item79 {
  const value: Item79 = {id, name};
  return value as Item79;
}
export const item79: Item79 = make79(79, "item-79");

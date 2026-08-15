interface Item162 { id: number; name: string; }
type Key162 = string | number;
export function make162(id: number, name: string): Item162 {
  const value: Item162 = {id, name};
  return value as Item162;
}
export const item162: Item162 = make162(162, "item-162");

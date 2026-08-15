interface Item304 { id: number; name: string; }
type Key304 = string | number;
export function make304(id: number, name: string): Item304 {
  const value: Item304 = {id, name};
  return value as Item304;
}
export const item304: Item304 = make304(304, "item-304");

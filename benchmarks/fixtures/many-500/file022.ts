interface Item22 { id: number; name: string; }
type Key22 = string | number;
export function make22(id: number, name: string): Item22 {
  const value: Item22 = {id, name};
  return value as Item22;
}
export const item22: Item22 = make22(22, "item-22");

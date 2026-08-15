interface Item85 { id: number; name: string; }
type Key85 = string | number;
export function make85(id: number, name: string): Item85 {
  const value: Item85 = {id, name};
  return value as Item85;
}
export const item85: Item85 = make85(85, "item-85");

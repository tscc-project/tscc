interface Item259 { id: number; name: string; }
type Key259 = string | number;
export function make259(id: number, name: string): Item259 {
  const value: Item259 = {id, name};
  return value as Item259;
}
export const item259: Item259 = make259(259, "item-259");

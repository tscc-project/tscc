interface Item382 { id: number; name: string; }
type Key382 = string | number;
export function make382(id: number, name: string): Item382 {
  const value: Item382 = {id, name};
  return value as Item382;
}
export const item382: Item382 = make382(382, "item-382");

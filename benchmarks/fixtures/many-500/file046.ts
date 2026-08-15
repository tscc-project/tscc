interface Item46 { id: number; name: string; }
type Key46 = string | number;
export function make46(id: number, name: string): Item46 {
  const value: Item46 = {id, name};
  return value as Item46;
}
export const item46: Item46 = make46(46, "item-46");

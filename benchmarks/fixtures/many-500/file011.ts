interface Item11 { id: number; name: string; }
type Key11 = string | number;
export function make11(id: number, name: string): Item11 {
  const value: Item11 = {id, name};
  return value as Item11;
}
export const item11: Item11 = make11(11, "item-11");

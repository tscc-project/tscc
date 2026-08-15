interface Item112 { id: number; name: string; }
type Key112 = string | number;
export function make112(id: number, name: string): Item112 {
  const value: Item112 = {id, name};
  return value as Item112;
}
export const item112: Item112 = make112(112, "item-112");

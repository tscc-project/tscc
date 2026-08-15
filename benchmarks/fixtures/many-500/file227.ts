interface Item227 { id: number; name: string; }
type Key227 = string | number;
export function make227(id: number, name: string): Item227 {
  const value: Item227 = {id, name};
  return value as Item227;
}
export const item227: Item227 = make227(227, "item-227");

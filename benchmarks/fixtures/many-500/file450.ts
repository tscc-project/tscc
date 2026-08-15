interface Item450 { id: number; name: string; }
type Key450 = string | number;
export function make450(id: number, name: string): Item450 {
  const value: Item450 = {id, name};
  return value as Item450;
}
export const item450: Item450 = make450(450, "item-450");

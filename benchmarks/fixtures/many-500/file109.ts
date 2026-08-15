interface Item109 { id: number; name: string; }
type Key109 = string | number;
export function make109(id: number, name: string): Item109 {
  const value: Item109 = {id, name};
  return value as Item109;
}
export const item109: Item109 = make109(109, "item-109");

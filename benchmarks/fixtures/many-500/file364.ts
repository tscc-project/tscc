interface Item364 { id: number; name: string; }
type Key364 = string | number;
export function make364(id: number, name: string): Item364 {
  const value: Item364 = {id, name};
  return value as Item364;
}
export const item364: Item364 = make364(364, "item-364");
